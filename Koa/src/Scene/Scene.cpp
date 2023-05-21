#include <array>

#include "Scene.h"

//Remove later
#include "../Core/Engine.h"
#include "../Core/Time.h"
#include "../Physics/PhysicsWorld.h"
#include "../Components/Renderers/SpriteRenderer.h"
#include "../Components/Renderers/PixelRenderer.h"
#include "../Components/Transform.h"
#include "../Components/Camera.h"
#include "../Components/Mesh.h"
#include "../Math/VectorOperations.h"
#include "../Math/Vector4.h"

Scene::Scene()
	:m_Name("None")
{
}

void Scene::Start()
{
	PhysicsWorld::CreateWorld();
}

void Scene::Update()
{
	static double duration = 0;
	duration += Time::DeltaTime();

	for (auto& entity : m_Entities)
	{
		if (entity->IsActive())
		{
			entity->ComponentsUpdate();
		}
	}

	//Fixed Update 
	if (duration >= Time::FixedDeltaTime())
	{
		PhysicsWorld::UpdateWorld();

		for (auto& entity : m_Entities)
		{
			if ((entity)->IsActive())
			{
				entity->ComponentsFixedUpdate();
			}
		}

		duration = 0;
	}
}

void Scene::Render()
{
	std::vector<Vector2f> square
	{
		{ -0.5f, -0.5f },
		{ -0.5f,  0.5f },
		{  0.5f, -0.5f },

		{  0.5f, -0.5f },
		{  0.5f,  0.5f },
		{ -0.5f,  0.5f }
	};

	//GetComponent of RenderType
	for (size_t i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities[i]->HasComponent<SpriteRenderer>())
		{
			std::vector<Vector2f> verticies(square);

			SpriteRenderer* spriteRenderer = m_Entities[i]->GetComponent<SpriteRenderer>();
			Matrix4x4f transformMatrix = m_Entities[i]->GetTransform()->GetTransformMatrix();
			for (size_t j = 0; j < verticies.size(); j++)
			{
				//verticies[j] = transformMatrix * Vector3f(verticies[j]);
				Vector4f vertex = transformMatrix * Vector4f(verticies[j], 0.0f, 1.0f);

				verticies[j].x = vertex.x;
				verticies[j].y = vertex.y;
			}

			Engine::Get().GetWindow()->DrawVerticies(std::vector<Vector2f>(verticies.begin(), verticies.begin() + 3), spriteRenderer->GetColor());
			Engine::Get().GetWindow()->DrawVerticies(std::vector<Vector2f>(verticies.begin() + 3, verticies.end()), spriteRenderer->GetColor());
		}
	}

	for (size_t i = 0; i < m_Entities.size(); i++)
	{
		if (m_Entities[i]->HasComponent<PixelRenderer>())
		{
			PixelRenderer* pixelRenderer = m_Entities[i]->GetComponent<PixelRenderer>();

			Vector3f position = pixelRenderer->GetTransform()->GetPosition();
			Engine::Get().GetWindow()->SetPixel(int(position.x), int(position.y), pixelRenderer->GetColor());
		}
	}

	Camera* camera = nullptr;
	for (const auto& entity : m_Entities)
	{
		if (entity->HasComponent<Camera>())
		{
			camera = entity->GetComponent<Camera>();
		}
	}

	std::vector<Vector3f> sprite
	{
		{ 0.5f,  0.5f, 10.0f },
		{ 0.5f, -0.5f, 10.0f },
		{ -0.5f, 0.5f, 10.0f }
	};

	if (camera != nullptr)
	{
		const Matrix4x4f& projection = camera->GetProjectionMatrix();

		for (auto& entity : m_Entities)
		{
			if (entity->HasComponent<Mesh>())
			{
				Mesh* mesh = entity->GetComponent<Mesh>();
				const std::vector<Vector3f>& meshVerticies = mesh->GetMeshVerticies();

				std::array<Vector3f, 3> polygon;
				auto it = polygon.begin();
				for (auto& v : meshVerticies)
				{
					Matrix4x4f meshMatrix = mesh->GetTransform()->GetTransformMatrix();
					Vector4f r1 = projection * meshMatrix * Vector4f(v, 1.0f);
					Vector4f r = r1;

					if (r.w != 0)
					{
						r /= r.w;
					}

					it->x = r.x;
					it->y = r.y;
					it->z = r.z;

					it++;
					if (it == polygon.end())
					{
						it = polygon.begin();
						Engine::Get().GetWindow()->DrawVerticies(polygon, Color::White);
					}
				}
			}
		}
	}
}

void Scene::Raytrace()
{

}

Entity* Scene::CreateEntity()
{
	m_Entities.push_back(std::make_unique<Entity>(this));
	return m_Entities.back().get();
}