#include "SDL_manager.h"


SDL_Window* SDL_manager::game_window;
std::vector<SDL_manager::callback> SDL_manager::mouse_down_callbacks;
std::vector<SDL_manager::callback> SDL_manager::mouse_up_callbacks;
std::vector<SDL_manager::callback> SDL_manager::window_close_callbacks;
std::vector<SDL_manager::callback> SDL_manager::r_down_callbacks;
std::vector<SDL_manager::callback> SDL_manager::f2_down_callbacks;
std::vector<SDL_manager::callback> SDL_manager::f1_down_callbacks;
std::vector<SDL_manager::callback> SDL_manager::esc_down_callbacks;
SDL_manager::mouse SDL_manager::mouse_state;
std::vector<HardInputEventType> SDL_manager::events;
SDL_Renderer* SDL_manager::renderer;


bool SDL_manager::initialised;

SDL_manager::SDL_manager()
{
}


SDL_manager::~SDL_manager()
{
}


void SDL_manager::initialise(std::string title, int width, int height, bool fullscreen)
{
	assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	if (!fullscreen)
		game_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN/* || SDL_WINDOW_FULLSCREEN*/);
	else
		game_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN || SDL_WINDOW_FULLSCREEN);

	//initialise window and renderer
	assert(game_window);
	renderer = SDL_CreateRenderer(game_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	assert(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BlendMode::SDL_BLENDMODE_ADD);
	//SDL_RenderSetScale(renderer, constants::scaling_horizontal, constants::scaling_vertical);

	//initialise img and mix libraries
	int img_flags = IMG_INIT_PNG;

	assert(IMG_Init(img_flags) & img_flags);
	assert(TTF_Init() != -1);
	assert(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) >= 0);
	initialised = true;

	//hide mouse cursor so the custom one can be drawn instead
	SDL_ShowCursor(0);
}


bool SDL_manager::change_resolution(int w_res, int h_res, bool fullscr)
{
	return false;
}

void SDL_manager::update_input()
{
	static bool last_r = false;
	static bool last_f1 = false;
	static bool last_f2 = false;
	static bool last_esc = false;
	//storing mouse clicks and mouse releases in mouse class
	//reading events such as windows X button pressed
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
		{
			events.push_back(HardInputEventType::window_close);
		}

		//storing mouse clicks and mouse releases in mouse class
		if (event.type == SDL_MOUSEBUTTONDOWN)//reading mouse events to determine when the click occurs
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				events.push_back(HardInputEventType::left_mouse_down);
				mouse_state.lmb_down = true;
			}
			else
			{
				events.push_back(HardInputEventType::right_mouse_down);
				mouse_state.rmb_down = true;
			}
		}
		else if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				events.push_back(HardInputEventType::left_mouse_up);
				mouse_state.lmb_down = false;
			}
			else
			{
				events.push_back(HardInputEventType::right_mouse_up);
				mouse_state.rmb_down = false;
			}
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_r)
			{
				if (!last_r)
					events.push_back(HardInputEventType::r_pressed);

				last_r = true;
				
			}
			else if (event.key.keysym.sym == SDLK_F1)
			{
				if (!last_f1)
					events.push_back(HardInputEventType::f1_pressed);

				last_f1 = true;
			}
			else if (event.key.keysym.sym == SDLK_F2)
			{
				if (!last_f2)
					events.push_back(HardInputEventType::f2_pressed);

				last_f2 = true;
			}
			else if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				if (!last_esc)
					events.push_back(HardInputEventType::esc_pressed);

				last_esc = true;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_r)
				last_r = false;
			else if (event.key.keysym.sym == SDLK_F1)
				last_f1 = false;
			else if (event.key.keysym.sym == SDLK_F2)
				last_f2 = false;
			else if (event.key.keysym.sym == SDLK_ESCAPE)
				last_esc = false;
		}
	}
	//keyboard_state = SDL_GetKeyboardState();
}

void SDL_manager::trigger_input_listeners()
{
	while (events.size() > 0)
	{
		HardInputEventType type = events.at(events.size()-1);
		switch (type)
		{
		case HardInputEventType::window_close:
			for (unsigned int i = 0; i < window_close_callbacks.size(); i++)
			{
				callback cb = window_close_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::left_mouse_down:
			for (unsigned int i = 0; i < mouse_down_callbacks.size(); i++)
			{
				callback cb = mouse_down_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::left_mouse_up:
			for (unsigned int i = 0; i < mouse_up_callbacks.size(); i++)
			{
				callback cb = mouse_up_callbacks.at(i);
				cb();
			}
			break;

		case HardInputEventType::r_pressed:
			for (unsigned int i = 0; i < r_down_callbacks.size(); i++)
			{
				callback cb = r_down_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::f1_pressed:
			for (unsigned int i = 0; i < f1_down_callbacks.size(); i++)
			{
				callback cb = f1_down_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::f2_pressed:
			for (unsigned int i = 0; i < f2_down_callbacks.size(); i++)
			{
				callback cb = f2_down_callbacks.at(i);
				cb();
			}
			break;
		case HardInputEventType::esc_pressed:
			for (unsigned int i = 0; i < esc_down_callbacks.size(); i++)
			{
				callback cb = esc_down_callbacks.at(i);
				cb();
			}
			break;
		}
		events.pop_back();
	}
}

void SDL_manager::render_sprite(SDL_Texture * texture, SDL_Rect dest, bool flip_x, bool flip_y)
{
	SDL_Point center = { dest.w / 2, dest.h / 2 };
	if(flip_x && flip_y)
		SDL_RenderCopyEx(SDL_manager::renderer, texture, nullptr, &dest, NULL, &center, (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL));
	else if(flip_x)
		SDL_RenderCopyEx(SDL_manager::renderer, texture, nullptr, &dest, NULL, &center, SDL_FLIP_HORIZONTAL);
	else if(flip_y)
			SDL_RenderCopyEx(SDL_manager::renderer, texture, nullptr, &dest, NULL, &center, SDL_FLIP_VERTICAL);
	else
		SDL_RenderCopy(SDL_manager::renderer, texture, nullptr, &dest);
}

void SDL_manager::render_sprite_src(SDL_Texture * texture, SDL_Rect src, SDL_Rect dest)
{
	if (dest.w == -1 && dest.h == -1)
	{
		SDL_manager::get_window_size(&dest.w, &dest.h);
	}
	SDL_RenderCopy(SDL_manager::renderer, texture, &src, &dest);
}

TTF_Font * SDL_manager::load_font(const char * path, int size, SDL_Color color)
{
	TTF_Font* result = TTF_OpenFont(path, size);
	assert(result);
	return result;
}

SDL_Texture* SDL_manager::load_texture(const char * path)
{
	SDL_Texture* temp = IMG_LoadTexture(renderer, path);
	//SDL_SetTextureBlendMode(temp, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	std::cout << IMG_GetError() << " for image " << path;
	if(!temp)
		temp = IMG_LoadTexture(renderer, "assets/graphics/objects/default.png");
	assert(temp);
	return temp;
}

SDL_Texture * SDL_manager::get_texture_from_text(const char*  text, SDL_Color color, TTF_Font* font, int max_width)
{
	SDL_Surface* temp_s;

	if (max_width == 0)
		temp_s = TTF_RenderText_Solid(font, text, { color.r, color.g, color.b, 255 });
	else
		temp_s = TTF_RenderText_Blended_Wrapped(font, text, { color.r, color.g, color.b, 255 }, max_width);
	assert(temp_s);

	SDL_Texture* temp_t = SDL_CreateTextureFromSurface(renderer, temp_s);

	SDL_FreeSurface(temp_s);
	return temp_t;
}

/*order:
0 - normal
1 - hover
2 - pressed
3 - blocked
*/
SDL_Texture * SDL_manager::get_spritesheet_from_sprites(std::vector<SDL_Texture*> sprites)
{
	if (sprites.size() < 1)
		return nullptr;

	int w, h;
	SDL_QueryTexture(sprites.at(0), NULL, NULL, &w, &h);
	SDL_Rect dest_rect = { 0,0,w,h };

	SDL_Texture* result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w*sprites.size(), h);
	SDL_SetTextureBlendMode(result, SDL_BlendMode::SDL_BLENDMODE_BLEND);//set blend mode of texture so alpha is actually supported
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);//set the color to fill texture with
	SDL_SetRenderTarget(renderer, result);//set rendering target to texture
	SDL_RenderClear(renderer);//fill texture with previously set transparent black color

	for (unsigned int i = 0; i < sprites.size(); i++)
	{
		SDL_RenderCopy(renderer, sprites.at(i), NULL, &dest_rect);
		dest_rect.x += dest_rect.w;
	}

	SDL_SetRenderTarget(renderer, NULL);
	return result;
}


SDL_Texture* SDL_manager::get_sprite_from_spritesheet(SDL_Texture * texture, SDL_Rect src_rect)
{
	SDL_Texture* subtexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, src_rect.w, src_rect.h);//texture that will contain new sprite
	SDL_SetTextureBlendMode(subtexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetRenderTarget(renderer, subtexture);
	SDL_RenderClear(renderer);
	
	SDL_Rect rect = { 0,0, src_rect.w, src_rect.h };
	SDL_RenderCopy(renderer, texture, &src_rect, &rect);
	SDL_SetRenderTarget(renderer, NULL);
	return subtexture;
}

SDL_Texture * SDL_manager::create_terrain_texture(std::vector<SDL_manager::Tile> tiles, int width, int height)
{
	SDL_Texture* final_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width,height);
	SDL_SetRenderTarget(renderer, final_tex);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	for (Tile t : tiles)
	{
		SDL_RenderCopy(renderer, t.tex, NULL, &t.draw_rect);
	}
	SDL_SetRenderTarget(renderer, NULL);
	return final_tex;
}


SDL_Texture * SDL_manager::render_texture_on_texture(SDL_Texture * tex1, SDL_Texture * tex2, int dest_w, int dest_h, SDL_Rect tex2_draw_rect)
{
	SDL_Texture* combined = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dest_w, dest_h);//texture that will contain new sprite
	SDL_SetTextureBlendMode(combined, SDL_BlendMode::SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_SetRenderTarget(renderer, combined);
	SDL_RenderClear(renderer);

	if (tex2_draw_rect.x != -1)
		tex2_draw_rect.x = tex2_draw_rect.x;
	else
		tex2_draw_rect.x = dest_w / 2 - tex2_draw_rect.w / 2;

	if (tex2_draw_rect.y != -1)
		tex2_draw_rect.y = tex2_draw_rect.y;
	else
		tex2_draw_rect.y = dest_h / 2 - tex2_draw_rect.h / 2;

	SDL_RenderCopy(renderer, tex1, NULL, NULL);
	SDL_RenderCopy(renderer, tex2, NULL, &tex2_draw_rect);

	SDL_SetRenderTarget(renderer, NULL);

	return combined;
}

/*
function that creates a plain texture of given width, height and color
*/
SDL_Texture* SDL_manager::create_texture(int w, int h, SDL_Color color)
{
	SDL_Texture* temp = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);//create a texture
	Uint32* pixels = new Uint32[w * h];// array that holds all the pixel values for the texture
	memset(pixels, 255, w * h * sizeof(Uint32));//set all the pixels to max value
	SDL_UpdateTexture(temp, NULL, pixels, w * sizeof(Uint32)); //apply pixels to texture
	delete[] pixels;//remove the array from memory to avoid memory leak
	SDL_SetTextureColorMod(temp, color.r, color.g, color.b);//set color of the texture
	SDL_SetTextureBlendMode(temp, SDL_BLENDMODE_BLEND);
	return temp;
}
