# Anastasia 资源管理系统重构计划

## 文档版本
- **创建日期**: 2025-10-23
- **最后更新**: 2025-10-23
- **状态**: 规划阶段

---

## 1. 项目背景

### 1.1 当前问题
Anastasia项目目前使用手动Vulkan内存管理，存在以下问题：

- ❌ **手动内存分配**: 直接调用`vkAllocateMemory`/`vkBindBufferMemory`，容易泄漏
- ❌ **无统一资源管理**: Buffer/Image创建逻辑分散在`Device`类中
- ❌ **无状态追踪**: 无法追踪资源生命周期，难以调试内存泄漏
- ❌ **无资源状态抽象**: 直接操作`VkAccessFlags`和`VkImageLayout`，代码冗长易错
- ❌ **无Barrier自动化**: 需要手动计算pipeline stage和access mask
- ❌ **无GPU调试支持**: GPU崩溃时无法定位具体操作
- ❌ **ImageView手动管理**: 需要手动创建和销毁ImageView

### 1.2 目标系统 - Aphrodite资源管理架构
参考Aphrodite项目的资源管理系统，其核心优势：

✅ **统一资源接口**: `device->create<T>()`/`device->destroy<T>()`
✅ **VMA内存管理**: 自动处理内存分配、碎片优化、内存域选择
✅ **ResourceState抽象**: 高级状态抽象替代底层Vulkan枚举
✅ **自动Barrier插入**: 从ResourceState自动推导Access Flags和Image Layout
✅ **完整资源追踪**: ResourceStats自动追踪创建/销毁位置和泄漏检测
✅ **GPU调试支持**: Breadcrumb系统精确定位GPU崩溃点
✅ **自动ImageView管理**: Image对象自动创建和缓存ImageView
✅ **线程安全**: ThreadSafeObjectPool支持多线程并发

---

## 2. 架构设计

### 2.1 整体架构层次

```
┌─────────────────────────────────────────────────────┐
│                Application Layer                     │
│         device->create<Buffer>(...)                  │
│         cmdBuffer->insertBarrier(...)                │
└──────────────────┬──────────────────────────────────┘
                   │
    ┌──────────────┴──────────────┐
    ▼                             ▼
┌────────────────────┐    ┌──────────────────────┐
│  Resource Manager  │    │  State Manager       │
│  - Device          │    │  - ResourceState     │
│  - ObjectPool      │    │  - Barrier Helper    │
└─────────┬──────────┘    └──────────┬───────────┘
          │                          │
          ▼                          ▼
┌────────────────────┐    ┌──────────────────────┐
│  Memory Manager    │    │  Debug System        │
│  - VMAAllocator    │    │  - ResourceStats     │
│  - DeviceAllocator │    │  - BreadcrumbTracker │
└────────────────────┘    └──────────────────────┘
```

### 2.2 核心组件

#### A. ResourceHandle基类
```cpp
template<typename THandle, typename TCreateInfo>
class ResourceHandle {
    THandle m_handle;              // Vulkan原生句柄
    TCreateInfo m_createInfo;      // 创建信息
    std::string m_debugName;       // Debug名称
    Timer m_timer;                 // 生命周期计时
};
```

#### B. 资源类型定义
```cpp
// Buffer资源
class Buffer : public ResourceHandle<VkBuffer, BufferCreateInfo> {
    BufferCreateInfo {
        size_t size;
        BufferUsageFlags usage;
        MemoryDomain domain;
    };
};

// Image资源
class Image : public ResourceHandle<VkImage, ImageCreateInfo> {
    ImageCreateInfo {
        Extent3D extent;
        Format format;
        ImageUsageFlags usage;
        MemoryDomain domain;
        uint32_t mipLevels;
        uint32_t arraySize;
    };
    
    HashMap<Format, ImageView*> m_imageViewCache;  // 自动管理
};
```

#### C. ResourceState枚举
```cpp
enum class ResourceState : uint32_t {
    Undefined        = 0,
    General          = 0x00000001,
    UniformBuffer    = 0x00000002,
    VertexBuffer     = 0x00000004,
    IndexBuffer      = 0x00000008,
    ShaderResource   = 0x00000020,
    UnorderedAccess  = 0x00000040,
    RenderTarget     = 0x00000080,
    DepthStencil     = 0x00000100,
    CopySource       = 0x00000800,
    CopyDest         = 0x00000400,
    Present          = 0x00004000,
};
using ResourceStateFlags = Flags<ResourceState>;
```

#### D. Device资源池
```cpp
struct ResourcePool {
    // 内存管理
    std::unique_ptr<VMADeviceAllocator> deviceMemory;
    
    // CPU对象池
    ThreadSafeObjectPool<Buffer> buffer;
    ThreadSafeObjectPool<Image> image;
    ThreadSafeObjectPool<ImageView> imageView;
    
    // 调试追踪
    ResourceStats resourceStats;
};
```

---

## 3. 重构阶段规划

### 阶段 1: 基础设施搭建 (1-2周)

#### 1.1 添加基础类型和工具
- [ ] 创建 `src/api/resourcehandle.h` - ResourceHandle基类
- [ ] 创建 `src/api/gpuResource.h` - 资源相关枚举和结构体
  - MemoryDomain枚举
  - ResourceState枚举
  - BufferUsage/ImageUsage枚举
  - Format枚举转换
- [ ] 创建 `src/common/flags.h` - 位标志工具类
- [ ] 创建 `src/allocator/objectPool.h` - 对象池实现
  - ObjectPool (单线程)
  - ThreadSafeObjectPool (线程安全)

#### 1.2 集成VMA内存分配器
- [ ] 复制 `src/api/deviceAllocator.h` - 抽象接口
- [ ] 复制 `src/api/vulkan/vmaAllocator.h/.cpp` - VMA封装实现
- [ ] 更新 `CMakeLists.txt` - 确保VMA正确链接
- [ ] 更新 `cmake/External.cmake` - 配置VMA依赖

**关键配置:**
```cmake
# CMakeLists.txt
find_package(VulkanMemoryAllocator REQUIRED)
target_link_libraries(Anastasia PRIVATE GPUOpen::VulkanMemoryAllocator)
```

#### 1.3 状态转换工具函数
- [ ] 创建 `src/api/vulkan/vkUtils.h` 添加:
  - `getAccessFlags(ResourceState) -> VkAccessFlags`
  - `getImageLayout(ResourceState) -> VkImageLayout`
  - `determinePipelineStage(VkAccessFlags) -> VkPipelineStageFlags`
  - `getImageAspect(Format) -> VkImageAspectFlags`

**实现示例:**
```cpp
VkAccessFlags getAccessFlags(ResourceState state) {
    VkAccessFlags flags = 0;
    if (state & ResourceState::ShaderResource)
        flags |= VK_ACCESS_SHADER_READ_BIT;
    if (state & ResourceState::UnorderedAccess)
        flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    if (state & ResourceState::RenderTarget)
        flags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | 
                 VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    // ... 更多映射
    return flags;
}
```

---

### 阶段 2: 重构Device类 (2-3周)

#### 2.1 添加ResourcePool到Device
```cpp
// src/api/vulkan/device.h
class Device {
private:
    struct ResourcePool {
        std::unique_ptr<VMADeviceAllocator> deviceMemory;
        ThreadSafeObjectPool<Buffer> buffer;
        ThreadSafeObjectPool<Image> image;
        ThreadSafeObjectPool<ImageView> imageView;
    } m_resourcePool;
    
public:
    // 统一创建接口
    template<typename TCreateInfo, typename TResource>
    Expected<TResource*> create(TCreateInfo&& createInfo, 
                                 std::string_view debugName = "");
    
    // 统一销毁接口
    template<typename TResource>
    void destroy(TResource* pResource);
};
```

#### 2.2 实现资源创建逻辑
- [ ] 重构 `createBuffer()` 方法
  - 使用ObjectPool分配Buffer对象
  - 使用VMADeviceAllocator分配GPU内存
  - 自动绑定内存
- [ ] 重构 `createImage()` 方法
  - 使用ObjectPool分配Image对象
  - 使用VMADeviceAllocator分配GPU内存
  - 自动绑定内存
- [ ] 实现 `createImageView()` 方法
  - 在Image内部自动创建和缓存

**新的创建流程:**
```cpp
template<>
Expected<Buffer*> Device::createImpl(const BufferCreateInfo& info) {
    // 1. 创建Vulkan Buffer
    VkBufferCreateInfo vkInfo = toVulkan(info);
    VkBuffer vkBuffer;
    vkCreateBuffer(device_, &vkInfo, nullptr, &vkBuffer);
    
    // 2. 从对象池分配包装对象
    Buffer* buffer = m_resourcePool.buffer.allocate(info, vkBuffer);
    
    // 3. 使用VMA分配GPU内存
    m_resourcePool.deviceMemory->allocate(buffer);
    
    return buffer;
}
```

#### 2.3 移除旧的手动内存管理代码
- [ ] 删除 `Device::findMemoryType()`
- [ ] 删除 `Device::createBuffer()` 中的手动内存分配逻辑
- [ ] 删除 `Device::createImageWithInfo()` 中的手动内存分配逻辑

---

### 阶段 3: 重构Buffer和Image类 (1-2周)

#### 3.1 创建新的Buffer类
```cpp
// src/api/vulkan/buffer.h
struct BufferCreateInfo {
    size_t size;
    BufferUsageFlags usage;
    MemoryDomain domain = MemoryDomain::Auto;
};

class Buffer : public ResourceHandle<VkBuffer, BufferCreateInfo> {
public:
    size_t getSize() const { return m_createInfo.size; }
    BufferUsageFlags getUsage() const { return m_createInfo.usage; }
};
```

#### 3.2 创建新的Image类
```cpp
// src/api/vulkan/image.h
struct ImageCreateInfo {
    Extent3D extent;
    Format format;
    ImageUsageFlags usage;
    MemoryDomain domain = MemoryDomain::Auto;
    uint32_t mipLevels = 1;
    uint32_t arraySize = 1;
    ImageType imageType = ImageType::e2D;
};

class Image : public ResourceHandle<VkImage, ImageCreateInfo> {
public:
    // 自动管理ImageView
    ImageView* getView(Format format = Format::Undefined);
    
    uint32_t getWidth() const { return m_createInfo.extent.width; }
    uint32_t getHeight() const { return m_createInfo.extent.height; }
    Format getFormat() const { return m_createInfo.format; }
    
private:
    Device* m_pDevice;
    HashMap<Format, ImageView*> m_imageViewCache;
    std::mutex m_viewCacheLock;
};
```

#### 3.3 实现ImageView自动管理
```cpp
ImageView* Image::getView(Format format) {
    if (format == Format::Undefined)
        format = m_createInfo.format;
    
    std::lock_guard lock(m_viewCacheLock);
    
    // 检查缓存
    if (!m_imageViewCache.contains(format)) {
        ImageViewCreateInfo viewInfo{
            .viewType = determineViewType(m_createInfo.imageType),
            .format = format,
            .pImage = this
        };
        
        auto result = m_pDevice->create<ImageView>(viewInfo);
        m_imageViewCache[format] = result.value();
    }
    
    return m_imageViewCache[format];
}
```

---

### 阶段 4: CommandBuffer Barrier自动化 (2-3周)

#### 4.1 添加Barrier结构体
```cpp
// src/api/vulkan/commandBuffer.h
struct BufferBarrier {
    Buffer* pBuffer;
    ResourceState currentState;
    ResourceState newState;
    QueueType queueType = QueueType::Graphics;
    bool acquire = false;
    bool release = false;
};

struct ImageBarrier {
    Image* pImage;
    ResourceState currentState;
    ResourceState newState;
    QueueType queueType = QueueType::Graphics;
    bool acquire = false;
    bool release = false;
    uint32_t mipLevel = 0;
    uint32_t arrayLayer = 0;
    bool subresourceBarrier = false;
};
```

#### 4.2 实现自动Barrier插入
```cpp
class CommandBuffer {
public:
    // 简化的状态转换API
    void transitionImageLayout(Image* image, 
                               ResourceState newState);
    void transitionImageLayout(Image* image,
                               ResourceState currentState,
                               ResourceState newState);
    
    // 批量Barrier
    void insertBarrier(ArrayProxy<BufferBarrier> bufferBarriers);
    void insertBarrier(ArrayProxy<ImageBarrier> imageBarriers);
    void insertBarrier(ArrayProxy<BufferBarrier> bufferBarriers,
                       ArrayProxy<ImageBarrier> imageBarriers);

private:
    void insertBarrierImpl(ArrayProxy<BufferBarrier> bufferBarriers,
                           ArrayProxy<ImageBarrier> imageBarriers);
};
```

#### 4.3 实现Barrier转换逻辑
```cpp
void CommandBuffer::insertBarrierImpl(
    ArrayProxy<BufferBarrier> bufferBarriers,
    ArrayProxy<ImageBarrier> imageBarriers) {
    
    std::vector<VkImageMemoryBarrier> vkImageBarriers;
    std::vector<VkBufferMemoryBarrier> vkBufferBarriers;
    
    // Image Barriers
    for (const auto& barrier : imageBarriers) {
        VkImageMemoryBarrier vkBarrier{};
        vkBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        
        // 自动推导Access Flags
        vkBarrier.srcAccessMask = getAccessFlags(barrier.currentState);
        vkBarrier.dstAccessMask = getAccessFlags(barrier.newState);
        
        // 自动推导Layout
        vkBarrier.oldLayout = getImageLayout(barrier.currentState);
        vkBarrier.newLayout = getImageLayout(barrier.newState);
        
        // 处理队列传输
        if (barrier.acquire) {
            vkBarrier.srcQueueFamilyIndex = getSrcQueueFamily(barrier.queueType);
            vkBarrier.dstQueueFamilyIndex = getCurrentQueueFamily();
        } else {
            vkBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            vkBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }
        
        vkBarrier.image = barrier.pImage->getHandle();
        vkBarrier.subresourceRange = {
            getImageAspect(barrier.pImage->getFormat()),
            barrier.mipLevel,
            barrier.subresourceBarrier ? 1 : VK_REMAINING_MIP_LEVELS,
            barrier.arrayLayer,
            barrier.subresourceBarrier ? 1 : VK_REMAINING_ARRAY_LAYERS
        };
        
        vkImageBarriers.push_back(vkBarrier);
    }
    
    // Buffer Barriers (类似逻辑)
    // ...
    
    // 自动推导Pipeline Stage
    VkPipelineStageFlags srcStage = determineSrcStage(imageBarriers, bufferBarriers);
    VkPipelineStageFlags dstStage = determineDstStage(imageBarriers, bufferBarriers);
    
    vkCmdPipelineBarrier(
        m_commandBuffer,
        srcStage, dstStage, 0,
        0, nullptr,
        vkBufferBarriers.size(), vkBufferBarriers.data(),
        vkImageBarriers.size(), vkImageBarriers.data()
    );
}
```

---

### 阶段 5: 调试和追踪系统 (2-3周)

#### 5.1 实现ResourceStats
```cpp
// src/api/vulkan/resourceStats.h
class ResourceStats {
public:
    enum class ResourceType {
        Buffer, Image, ImageView, Sampler, /* ... */
    };
    
    template<typename T>
    void trackCreation(const std::source_location& location);
    
    template<typename T>
    void trackDestruction(const std::source_location& location);
    
    std::string generateReport() const;
    
private:
    struct LocationInfo {
        std::string file;
        uint32_t line;
        uint32_t count;
    };
    
    HashMap<ResourceType, uint32_t> m_created;
    HashMap<ResourceType, uint32_t> m_destroyed;
    HashMap<ResourceType, uint32_t> m_active;
    HashMap<ResourceType, SmallVector<LocationInfo>> m_creationLocations;
};
```

#### 5.2 集成到Device
```cpp
class Device {
    template<typename T>
    Expected<T*> create(/*...*/) {
        auto result = createImpl(/*...*/);
        if (result.success()) {
            // 自动追踪
            m_resourceStats.trackCreation<T>(
                std::source_location::current()
            );
        }
        return result;
    }
    
    template<typename T>
    void destroy(T* resource) {
        m_resourceStats.trackDestruction<T>(
            std::source_location::current()
        );
        destroyImpl(resource);
    }
};
```

#### 5.3 实现BreadcrumbTracker (可选,高级功能)
```cpp
// src/common/breadcrumbTracker.h
enum class BreadcrumbState {
    Pending,
    InProgress,
    Completed,
    Failed
};

class BreadcrumbTracker {
public:
    uint32_t addBreadcrumb(std::string_view operation,
                           std::string_view details,
                           uint32_t parentIndex = UINT32_MAX);
    
    void updateBreadcrumb(uint32_t index, BreadcrumbState state);
    
    std::string toString(std::string_view prefix = "") const;
    
private:
    struct Breadcrumb {
        std::string operation;
        std::string details;
        BreadcrumbState state;
        uint32_t parentIndex;
        std::vector<uint32_t> children;
    };
    
    std::vector<Breadcrumb> m_breadcrumbs;
    bool m_enabled = false;
};
```

#### 5.4 集成到CommandBuffer
```cpp
class CommandBuffer {
public:
    void draw(DrawArguments args) {
        uint32_t idx = m_breadcrumbs.addBreadcrumb(
            "Draw",
            std::format("VertexCount={}", args.vertexCount)
        );
        
        m_breadcrumbs.updateBreadcrumb(idx, BreadcrumbState::InProgress);
        
        // 实际draw调用
        flushGraphicsCommand();
        vkCmdDraw(...);
        
        m_breadcrumbs.updateBreadcrumb(idx, BreadcrumbState::Completed);
    }
    
    std::string generateBreadcrumbReport() const;
    
private:
    BreadcrumbTracker m_breadcrumbs;
};
```

---

### 阶段 6: 迁移现有代码 (3-4周)

#### 6.1 更新Model类
- [ ] 修改 `src/api/vulkan/model.h` - 使用新的Buffer API
- [ ] 修改 `src/api/vulkan/model.cpp` - 替换手动内存管理

**迁移示例:**
```cpp
// 旧代码
void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    device.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );
    // ... 复杂的内存映射和复制逻辑
}

// 新代码
void Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
    size_t bufferSize = sizeof(vertices[0]) * vertices.size();
    
    // 1. 创建staging buffer
    auto stagingBuffer = device->create<Buffer>(BufferCreateInfo{
        .size = bufferSize,
        .usage = BufferUsage::TransferSrc,
        .domain = MemoryDomain::Upload  // 自动HOST_VISIBLE
    }, "VertexStagingBuffer");
    
    // 2. 映射和复制
    void* data = device->mapMemory(stagingBuffer.value());
    memcpy(data, vertices.data(), bufferSize);
    device->unMapMemory(stagingBuffer.value());
    
    // 3. 创建设备本地buffer
    auto vertexBuffer = device->create<Buffer>(BufferCreateInfo{
        .size = bufferSize,
        .usage = BufferUsage::Vertex | BufferUsage::TransferDst,
        .domain = MemoryDomain::Device
    }, "VertexBuffer");
    
    // 4. 复制 (自动barrier)
    cmdBuffer->copy(stagingBuffer.value(), vertexBuffer.value(), 
                    Range{0, bufferSize});
    
    // 5. 清理
    device->destroy(stagingBuffer.value());
    
    m_vertexBuffer = vertexBuffer.value();
}
```

#### 6.2 更新SwapChain类
- [ ] 修改 `src/api/vulkan/swapchain.h` - 使用新的Image API
- [ ] 修改 `src/api/vulkan/swapchain.cpp` - 自动ImageView管理

#### 6.3 更新Renderer类
- [ ] 修改 `src/api/vulkan/renderer.cpp` - 使用新的Barrier API
- [ ] 简化渲染流程代码

**迁移示例:**
```cpp
// 旧代码 - 手动barrier (15+行)
void Renderer::transitionImageLayout(VkImage image, /*...*/) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    // ... 更多配置
    
    VkPipelineStageFlags srcStage, dstStage;
    // ... 复杂的stage判断逻辑
    
    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, /*...*/);
}

// 新代码 - 一行调用
void Renderer::prepareTextureForShader(Image* texture) {
    cmdBuffer->transitionImageLayout(texture,
        ResourceState::CopyDest,
        ResourceState::ShaderResource);
}
```

---

### 阶段 7: 测试和优化 (2-3周)

#### 7.1 单元测试
- [ ] 测试ObjectPool分配和释放
- [ ] 测试VMADeviceAllocator内存分配
- [ ] 测试ResourceState转换正确性
- [ ] 测试Barrier自动插入
- [ ] 测试ImageView自动管理

#### 7.2 集成测试
- [ ] 测试完整渲染流程
- [ ] 测试资源生命周期
- [ ] 测试内存泄漏检测
- [ ] 压力测试多线程场景

#### 7.3 性能验证
- [ ] 对比旧版本渲染性能
- [ ] 验证VMA内存利用率
- [ ] 检查对象池开销
- [ ] Profile关键路径

#### 7.4 调试工具验证
- [ ] 触发GPU崩溃，验证Breadcrumb报告
- [ ] 人为制造内存泄漏，验证ResourceStats报告
- [ ] 验证debug名称在RenderDoc中显示

---

## 4. 风险评估与缓解

### 4.1 主要风险

| 风险 | 严重性 | 概率 | 缓解措施 |
|------|--------|------|----------|
| VMA集成导致编译错误 | 高 | 中 | 提前测试VMA单独编译，准备降级方案 |
| API变化破坏现有代码 | 高 | 高 | 保留旧Device类，创建Device2新类过渡 |
| 性能回退 | 中 | 低 | 每阶段性能对比，保留性能基准测试 |
| Barrier自动化bug | 高 | 中 | 详细单元测试，RenderDoc验证 |
| 内存泄漏未检测到 | 中 | 中 | 使用Valgrind/ASan双重验证 |
| 线程安全问题 | 中 | 低 | ThreadSanitizer检测，限制并发场景 |

### 4.2 回滚策略
- 每个阶段独立分支开发 (`refactor/phase-N`)
- 保留原始Device类为`DeviceLegacy`
- 使用宏开关切换新旧实现
```cpp
#ifdef ANA_USE_NEW_RESOURCE_SYSTEM
    using Device = Device2;
#else
    using Device = DeviceLegacy;
#endif
```

---

## 5. 时间线和里程碑

### 总体时间: 14-20周 (约3.5-5个月)

```
Week 1-2:   阶段1 - 基础设施搭建
Week 3-5:   阶段2 - 重构Device类
Week 6-7:   阶段3 - 重构Buffer和Image类
Week 8-10:  阶段4 - CommandBuffer Barrier自动化
Week 11-13: 阶段5 - 调试和追踪系统
Week 14-17: 阶段6 - 迁移现有代码
Week 18-20: 阶段7 - 测试和优化
```

### 关键里程碑
- ✅ **M1 (Week 2)**: VMA成功集成，基础类型完成
- ✅ **M2 (Week 5)**: Device统一资源接口可用
- ✅ **M3 (Week 10)**: Barrier自动化完成，手动barrier代码可删除
- ✅ **M4 (Week 13)**: 调试系统完成，可定位资源泄漏
- ✅ **M5 (Week 17)**: 所有旧代码迁移完成
- ✅ **M6 (Week 20)**: 性能验证通过，正式发布

---

## 6. 成功标准

### 6.1 功能标准
- ✅ 所有资源通过统一`device->create<T>()`接口创建
- ✅ VMA管理所有GPU内存，无手动`vkAllocateMemory`调用
- ✅ Barrier一行代码完成，自动推导Access/Layout/Stage
- ✅ ImageView自动创建和缓存，无手动管理
- ✅ 资源泄漏自动检测并报告位置

### 6.2 性能标准
- ✅ 渲染性能不低于旧版本 (允许±3%误差)
- ✅ 内存占用不增加超过10%
- ✅ 资源创建开销不增加超过20%

### 6.3 代码质量标准
- ✅ 核心路径代码行数减少40%+
- ✅ 所有公共API有完整注释
- ✅ 单元测试覆盖率 > 80%
- ✅ 无Valgrind/ASan报错

---

## 7. 依赖和前置条件

### 7.1 技术依赖
- C++20 支持 (std::source_location)
- CMake 3.28+
- Vulkan SDK 1.3+
- VulkanMemoryAllocator 3.0+

### 7.2 知识储备
- 深入理解Vulkan内存管理
- 熟悉VMA使用
- 理解Vulkan同步和Barrier机制
- 了解对象池设计模式

---

## 8. 参考资料

### 8.1 外部资源
- [Vulkan Memory Allocator Documentation](https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/)
- [Vulkan Synchronization Primer](https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples)
- [Aphrodite 项目源码](file:///home/mingshi/Project/GPU/Aphrodite)

### 8.2 内部文档
- Anastasia 当前架构文档
- Aphrodite 资源管理分析文档 (本文档)

---

## 9. 附录

### 9.1 关键API对比

#### 创建Buffer
```cpp
// 旧API (手动管理)
VkBuffer buffer;
VkDeviceMemory memory;
device.createBuffer(size, usage, properties, buffer, memory);

// 新API (自动管理)
auto buffer = device->create<Buffer>(BufferCreateInfo{
    .size = size,
    .usage = BufferUsage::Vertex,
    .domain = MemoryDomain::Device
}, "MyVertexBuffer");
```

#### Image Layout Transition
```cpp
// 旧API (15+行)
VkImageMemoryBarrier barrier{};
barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
barrier.srcAccessMask = 0;
barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
// ... 更多字段
vkCmdPipelineBarrier(cmdBuffer, srcStage, dstStage, ...);

// 新API (1行)
cmdBuffer->transitionImageLayout(image, 
    ResourceState::Undefined,
    ResourceState::ShaderResource);
```

### 9.2 目录结构变化

```
Anastasia/
├── src/
│   ├── api/
│   │   ├── resourcehandle.h           # [NEW] 资源基类
│   │   ├── gpuResource.h              # [NEW] 资源类型定义
│   │   ├── deviceAllocator.h          # [NEW] 分配器接口
│   │   └── vulkan/
│   │       ├── device.h               # [MODIFIED] 统一资源管理
│   │       ├── buffer.h               # [MODIFIED] 新Buffer类
│   │       ├── image.h                # [MODIFIED] 新Image类
│   │       ├── vmaAllocator.h         # [NEW] VMA封装
│   │       ├── resourceStats.h        # [NEW] 资源追踪
│   │       └── vkUtils.h              # [MODIFIED] 状态转换工具
│   ├── allocator/
│   │   ├── objectPool.h               # [NEW] 对象池
│   │   └── allocator.h                # [EXISTING] CPU内存分配
│   ├── common/
│   │   ├── flags.h                    # [NEW] 位标志工具
│   │   └── breadcrumbTracker.h        # [NEW] GPU调试工具
│   └── ...
└── doc/
    └── REFACTOR_PLAN.md               # [THIS FILE]
```

---

**文档维护者**: @mingshi  
**审核者**: TBD  
**下次审核日期**: 2025-11-01
