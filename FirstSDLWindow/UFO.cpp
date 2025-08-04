#include "UFO.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Texture* UFO::textures;
std::vector<SDL_Texture*> UFO::deathTextures;

UFO::UFO(SDL_Renderer* renderer)
	:renderer(renderer), active(false), speed(4)
{
	rect = { -60, 100, 50, 25 };
}

void UFO::LoadTextures(SDL_Renderer* renderer)
{
	std::string texturePath = "../Assets/Textures/ufo.png";
	SDL_Surface* surface = IMG_Load(texturePath.c_str());
	if (!surface)
		SDL_Log("Failed to load enemy image: %s", IMG_GetError());

	textures = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);


	std::vector<std::string> deathPaths =
	{
		"../Assets/Textures/enemy_death_1.png",
		"../Assets/Textures/enemy_death_2.png"
	};

	for (const std::string& path : deathPaths)
	{
		SDL_Surface* surface = IMG_Load(path.c_str());
		if (!surface)
		{
			SDL_Log("Failed to load death image: %s", IMG_GetError());
			continue;
		}

		SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		if (tex)
			deathTextures.push_back(tex);
	}
}

void UFO::FreeTextures()
{
	SDL_DestroyTexture(textures);

	for (SDL_Texture* tex : deathTextures)
	{
		SDL_DestroyTexture(tex);
	}
	deathTextures.clear();
}

void UFO::update(float deltaTime)
{
	if (dying && !finishedDeathAnimation)
	{
		deathTimer += deltaTime;

		if (deathTimer >= deathFrameDuration)
		{
			deathTimer = 0.0f;
			deathFrameIndex++;

			if (deathFrameIndex >= static_cast<int>(deathTextures.size()))
			{
				finishedDeathAnimation = true;
			}
		}
	}

	if (!active) return;

	rect.x += speed;

	if (rect.x > SCREEN_WIDTH)
		deactivate();

}

void UFO::render()
{
	if (active)
	{
		if (textures)
			SDL_RenderCopy(renderer, textures, nullptr, &rect);
		else
		{
			// Fallback if texture isn't loaded
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	if (dying)
	{
		if (!finishedDeathAnimation && deathFrameIndex < deathTextures.size())
		{
			SDL_Texture* currentDeathFrame = deathTextures[deathFrameIndex];
			SDL_RenderCopy(renderer, currentDeathFrame, nullptr, &rect);
		}
		return; // Do not render normal ufo texture
	}

}

void UFO::reset()
{
	rect.x = -rect.w;
	active = true;
}

void UFO::startDeathAnimation()
{
	dying = true;
	finishedDeathAnimation = false;
	deathFrameIndex = 0;
	deathTimer = 0.0f;
}

void UFO::activate(){ reset(); }
void UFO::deactivate() { active = false; }
bool UFO::isActive() const { return active; }
SDL_Rect UFO::getRect() const{return rect;}
int UFO::getX() const { return rect.x + rect.w / 2; }
int UFO::getY() const { return rect.y; }