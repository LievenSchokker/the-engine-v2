#pragma once


#include "RenderQueueBase.h"
#include "IRenderQueueWriter.h"
#include "IUserInterfaceRenderQueueWriter.h"
#include "Rendering/RenderCommand.h"
#include "Rendering/UIRenderCommand.h"

/**
 * @brief Collects and organizes render commands into separate world and UI streams.
 *
 * Splitting world geometry from UI into distinct queues allows each to use
 * appropriate sorting strategies. Keeping both queues in one class lets the
 * render system manage frame lifecycle (collect, sort, draw, clear) in a
 * single place without scattering that logic across subsystems.
 *
 * The class implements narrow writer interfaces so that scene objects can only
 * append commands—never read, reorder, or clear them. This prevents accidental
 * corruption of the queue mid-frame and makes the data flow explicit: objects
 * produce commands, RenderSystem consumes them.
 *
 */
class RenderQueue : public IRenderQueueWriter,
                    public IUserInterfaceRenderQueueWriter
{
public:
    /// @name Writer interface implementations
    /// @{
    /// This make sure components can have WriteOnly acces
    /// and cannot delete nor Read commands.
    void push(RenderCommand command) override;
    void push(UIRenderCommand command) override;
    /// @}

    /**
     * @brief Sorts both queues using their respective strategies.
     *
     * Called once per frame after collection is complete. Batching the sort
     * avoids repeated partial sorts as commands trickle in.
     */
    void sortAll();

    /**
     * @brief Prepares queues for the next frame.
     *
     * Reusing allocated memory across frames reduces allocator pressure
     * compared to destroying and recreating containers each frame.
     *
     */
    void clearAll();

    /**
     * @brief Provides read access for the renderer.
     *
     * Only the render system should call these; components interact
     * exclusively through the writer interfaces.
     *
     */
    RenderQueueBase<RenderCommand>& world();
    RenderQueueBase<UIRenderCommand>& ui();
private:
    RenderQueueBase<RenderCommand> worldQueue;
    RenderQueueBase<UIRenderCommand> uiQueue;
};