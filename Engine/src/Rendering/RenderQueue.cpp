#include "../../inc/Rendering/RenderQueue.h"

#include "../../inc/Rendering/IRenderer.h"

void executeRenderQueue(IRenderer& renderer, const RenderQueue& queue)
{
	renderer.beginFrame(queue.clearColor);

	for ( const auto& command : queue.shapes ) {
		switch ( command.type ) {
			case ShapeRenderType::Circle:
				renderer.drawCircle(command.position, command.radius,
									command.color, command.scale);
				break;
			case ShapeRenderType::Rectangle:
				renderer.drawRectangle(command.position, command.size,
									   command.rotationDegrees, command.color,
									   command.scale);
				break;
			case ShapeRenderType::None:
			default:
				break;
		}
	}

	renderer.presentFrame();
}
