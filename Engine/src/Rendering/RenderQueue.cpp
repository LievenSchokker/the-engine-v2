#include "Rendering/RenderQueue.h"

#include "Rendering/IRenderer.h"

void executeRenderQueue(IRenderer& renderer, const RenderQueue& queue)
{
	renderer.beginFrame(queue.clearColor);

	for ( const auto& command : queue.shapes )
	{
		switch ( command.type )
		{
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

	for ( const auto& command : queue.sprites )
	{
		if ( command.sprite != nullptr )
		{
			renderer.drawSprite(command.position, command.size, command.sprite,
								&command.srcRect, command.rotationDegrees,
								command.scale, command.tint, command.flipX,
								command.flipY);
		}
	}

	renderer.presentFrame();
}
