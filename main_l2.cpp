#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
#include <iostream>
#include "imgui-SFML.h"
#include "imgui.h"

enum class ColorPalette
{
	GRADIENT,
	GRAYSCALE,
	RED_BLUE
};

class RFuncSprite : public sf::Sprite
{
public:
	void Create(const sf::Vector2u &size);
	void DrawRFunc(const std::function<float(const sf::Vector2f &)> &rfunc, const sf::FloatRect &subSpace,
				   ColorPalette selectedPalette);
	sf::Texture _texture;

private:
	sf::Image _image;
};

void RFuncSprite::Create(const sf::Vector2u &size)
{
	_image.create(size.x, size.y, sf::Color::Cyan);
	_texture.loadFromImage(_image);
	setTexture(_texture);
}

void RFuncSprite::DrawRFunc(const std::function<float(const sf::Vector2f &)> &rfunc, const sf::FloatRect &subSpace,
							ColorPalette selectedPalette)
{
	sf::Vector2f spaceStep = {subSpace.width / static_cast<float>(_image.getSize().x),
							  subSpace.height / static_cast<float>(_image.getSize().y)};

	for (unsigned x = 0; x < _image.getSize().x; ++x)
	{
		for (unsigned y = 0; y < _image.getSize().y; ++y)
		{
			sf::Vector2f spacePoint1 = {subSpace.left + static_cast<float>(x) * spaceStep.x,
										subSpace.top + static_cast<float>(y) * spaceStep.y};

			sf::Vector2f spacePoint2 = {subSpace.left + static_cast<float>(x + 1) * spaceStep.x,
										subSpace.top + static_cast<float>(y) * spaceStep.y};

			sf::Vector2f spacePoint3 = {subSpace.left + static_cast<float>(x) * spaceStep.x,
										subSpace.top + static_cast<float>(y + 1) * spaceStep.y};

			const float z1 = rfunc(spacePoint1);
			const float z2 = rfunc(spacePoint2);
			const float z3 = rfunc(spacePoint3);

			float nx = z1 - z2;
			float ny = z1 - z3;
			float nz = 2.0f;
			float nw = sqrt(nx * nx + ny * ny + nz * nz);

			nx /= nw;
			ny /= nw;
			nz /= nw;

			sf::Color pixelColor;

			switch (selectedPalette)
			{
			case ColorPalette::GRADIENT:
				pixelColor.r = static_cast<sf::Uint8>((nx + 1.f) / 2.f * 255);
				pixelColor.g = static_cast<sf::Uint8>((ny + 1.f) / 2.f * 255);
				pixelColor.b = static_cast<sf::Uint8>((nz + 1.f) / 2.f * 255);
				break;
			case ColorPalette::GRAYSCALE:
				pixelColor.r = pixelColor.g = pixelColor.b = static_cast<sf::Uint8>((nx + 1.f) / 2.f * 255);
				break;
			case ColorPalette::RED_BLUE:
				pixelColor.r = static_cast<sf::Uint8>((nx + 1.f) / 2.f * 255);
				pixelColor.b = static_cast<sf::Uint8>((ny + 1.f) / 2.f * 255);
				pixelColor.g = 0;
				break;
			}

			_image.setPixel(x, y, pixelColor);
		}
	}

	_texture.loadFromImage(_image);
	setTexture(_texture);
}

float circleFunction(const sf::Vector2f &point)
{
	float radius = 200.0f;
	float centerX = 400.0f;
	float centerY = 300.0f;
	float distance = sqrt(pow(point.x - centerX, 2) + pow(point.y - centerY, 2));
	return abs(distance - radius);
}

float spiralFunction(const sf::Vector2f &point)
{
	float a = 0.1f;
	float b = 0.05f;
	return sin(sqrt(point.x * point.x + point.y * point.y)) * sin(a * sqrt(point.x * point.x + point.y * point.y) + b);
}

float waveFunction(const sf::Vector2f &point) { return sin(point.x * 0.1f) * sin(point.y * 0.1f); }

float cosineFunction(const sf::Vector2f &point) { return cos(point.x * 0.1f) * cos(point.y * 0.1f); }

float roseFunction(const sf::Vector2f &point)
{
	float k = 3.0f;
	float r = sin(k * (point.x * point.x + point.y * point.y));
	return r;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
	ImGui::SFML::Init(window);

	RFuncSprite rFuncSprite;
	rFuncSprite.Create(window.getSize());

	std::string functionNames[] = {"Circle", "Spiral", "Wave", "Cosine", "Rose"};
	int selectedFunction = 0;
	ColorPalette selectedPalette = ColorPalette::GRADIENT; // Выбранная палитра

	char fileName[256] = "result.png";

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		ImGui::SFML::Update(window, sf::seconds(1.f / 60.f));

		#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

class RoseShape : public sf::Drawable
{
public:
    RoseShape(unsigned int size) : size(size)
    {
        image.create(size, size, sf::Color::White);
        CreateRoseShape();
        texture.loadFromImage(image);
        sprite.setTexture(texture);
    }

    void CreateRoseShape()
    {
        for (unsigned int y = 0; y < size; ++y)
        {
            for (unsigned int x = 0; x < size; ++x)
            {
                float rx = x - size / 2;
                float ry = y - size / 2;
                float roseValue = roseFunction(rx, ry);
                if ((roseValue >= -0.1f && roseValue <= 0.1f) || (roseValue >= 0.9f && roseValue <= 1.1f) ||
                    (roseValue >= -1.1f && roseValue <= -0.9f))
                {
                    image.setPixel(x, y, sf::Color::Black);
                }
                else
                {
                    image.setPixel(x, y, sf::Color::White);
                }
            }
        }
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { target.draw(sprite, states); }

    void ApplyGradientDescent(float step)
    {
        for (unsigned int y = 0; y < size; ++y)
        {
            for (unsigned int x = 0; x < size; ++x)
            {
                sf::Color pixelColor = image.getPixel(x, y);
                pixelColor.r = static_cast<sf::Uint8>(pixelColor.r * (1.0f - step));
                pixelColor.g = static_cast<sf::Uint8>(pixelColor.g * (1.0f - step));
                pixelColor.b = static_cast<sf::Uint8>(pixelColor.b * (1.0f - step));
                image.setPixel(x, y, pixelColor);
            }
        }
        texture.loadFromImage(image);
        sprite.setTexture(texture);
    }

private:
    unsigned int size;
    sf::Image image;
    sf::Texture texture;
    sf::Sprite sprite;

    float roseFunction(float x, float y) const
    {
        float a = 300.0f;
        float b = 0.3f;
        return sin(a * sqrt(x * x + y * y)) * sin(b * atan2(y, x));
    }
};

int main()
{
    unsigned int windowSize = 600;
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize), "Rose-Shape & Gradient Descent");
    RoseShape roseShape(windowSize);

    bool showRoseShape = true;
    float gradientDescentStep = 0.01f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                    showRoseShape = !showRoseShape; // Переключение между лепестками розы и градиентным спуском
                if (event.key.code == sf::Keyboard::Up)
                    gradientDescentStep += 0.001f; // Увеличение шага градиентного спуска
                if (event.key.code == sf::Keyboard::Down)
                    gradientDescentStep -= 0.001f; // Уменьшение шага градиентного спуска
            }
        }

        window.clear(sf::Color::White);

        if (showRoseShape)
        {
            window.draw(roseShape);
        }
        else
        {
            roseShape.ApplyGradientDescent(gradientDescentStep);
            window.draw(roseShape);
        }

        window.display();
    }

    return 0;
}

		ImGui::End();

		switch (selectedFunction)
		{
		case 0:
			rFuncSprite.DrawRFunc(circleFunction, sf::FloatRect(0, 0, window.getSize().x, window.getSize().y),
								  selectedPalette);
			break;
		case 1:
			rFuncSprite.DrawRFunc(spiralFunction, sf::FloatRect(0, 0, window.getSize().x, window.getSize().y),
								  selectedPalette);
			break;
		case 2:
			rFuncSprite.DrawRFunc(waveFunction, sf::FloatRect(0, 0, window.getSize().x, window.getSize().y),
								  selectedPalette);
			break;
		case 3:
			rFuncSprite.DrawRFunc(cosineFunction, sf::FloatRect(0, 0, window.getSize().x, window.getSize().y),
								  selectedPalette);
			break;
		case 4:
			rFuncSprite.DrawRFunc(roseFunction, sf::FloatRect(0, 0, window.getSize().x, window.getSize().y),
								  selectedPalette);
			break;
		}

		window.clear();
		window.draw(rFuncSprite);
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}
