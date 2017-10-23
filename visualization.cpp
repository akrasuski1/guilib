#include "visualization.h"

#include <cmath>
#include <sstream>


Visualization::Visualization(size_t w, size_t h):
	window(sf::VideoMode(w, h), "Visualization"),
	is_running(true), 
	drawing_thread(&Visualization::loop, this),
	console_mode(false),
	scale(0.1)
{}

Visualization::~Visualization() {
	is_running = false;
	drawing_thread.join();
}

void Visualization::set_event_handler(std::function<void(sf::Event)> fun) {
	handle_event = fun;
}

void Visualization::set_command_handler(std::function<void(std::string)> fun) {
	handle_command = fun;
}

void Visualization::keyboard(sf::Event::KeyEvent key) {
	switch (key.code) {
	case sf::Keyboard::Tilde: {
		console_mode = !console_mode;
	} break;
	
	case sf::Keyboard::Return: {
		if (console_mode && handle_command) {
			handle_command(command);
			command = "";
		}
	}
	
	default: break;
	}
}

sf::Vector2f Visualization::get_mouse_pos() {
	return window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

void Visualization::loop() {
	window.setFramerateLimit(60);

	view.setCenter(0, 0);
	rescale();

	if (!font.loadFromFile("res/ttf/arial.ttf")) {
		throw std::runtime_error("Could not load font!");
	}

	bool dragging = false;
	sf::Clock clock;
	while (is_running && window.isOpen()) {
		std::stringstream title;
		title << "FPS: " << round(1.0 / clock.getElapsedTime().asSeconds());
		window.setTitle(title.str());
		clock.restart();

		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
			case sf::Event::Closed: {
				window.close();
			} break;

			case sf::Event::Resized: {
				rescale();
			} break;

			case sf::Event::KeyPressed: {
				keyboard(event.key);
			} break;

			case sf::Event::TextEntered: {
				if (console_mode) {
				   	if (event.text.unicode == '`') {}
					else if (event.text.unicode == '\r') {}
					else if (event.text.unicode == '\x08') { // Backspace.
						command = command.substr(0, command.size() - 1);
					}
					else {
						command += event.text.unicode;
					}
				}
			} break;

			case sf::Event::MouseWheelScrolled: {
				scale *= pow(0.95, event.mouseWheelScroll.delta);
				rescale();
			} break;

			case sf::Event::MouseButtonPressed: {
				if (event.mouseButton.button == sf::Mouse::Right) {
					drag_start = get_mouse_pos();
					dragging = true;
				}
			} break;

			case sf::Event::MouseButtonReleased: {
				if (event.mouseButton.button == sf::Mouse::Right) {
					dragging = false;
				}
			} break;

			case sf::Event::MouseMoved: {
				if (dragging) {
					view.setCenter(view.getCenter()
						   	+ (drag_start - get_mouse_pos()));
					window.setView(view);
				}
			} break;

			default: break;
			}

			if (handle_event) {
				handle_event(event);
			}
		}
		
		draw();
	}
}

void Visualization::rescale() {
	size_t x = window.getSize().x;
	size_t y = window.getSize().y;
	view.setSize(scale * x, scale * y);
	window.setView(view);
}

void Visualization::draw() {
	window.clear();
	{
		std::lock_guard<std::mutex> lock(mtx);

		sf::FloatRect wbox(
				view.getCenter().x - view.getSize().x / 2,
				view.getCenter().y - view.getSize().y / 2,
				view.getSize().x, view.getSize().y);

		for (const auto& sh: shapes) {
			// Only draw what's visible...
			if (wbox.intersects(sh->getGlobalBounds())) {
				window.draw(*sh);
			}
		}
	}

	if (console_mode) {
		window.setView(window.getDefaultView());

		sf::RectangleShape bg(sf::Vector2f(window.getSize().x, 20));
		bg.setFillColor(sf::Color::White);
		window.draw(bg);

		sf::Text text;
		text.setFont(font);
		text.setString(command);
		text.setCharacterSize(14);
		text.setFillColor(sf::Color::Black);
		text.setOrigin(0, 0);
		text.setPosition(10, 0);
		window.draw(text);

		window.setView(view);
	}
	window.display();
}

void Visualization::add_circle(
		sf::Vector2f pos, float d, sf::Color color, size_t edges) {
	auto shape = std::make_unique<sf::CircleShape>(d / 2, edges);
	shape->setOrigin({d / 2, d / 2});
	shape->setFillColor(color);
	shape->setPosition(pos);
	next_shapes.push_back(std::move(shape));
}

void Visualization::add_triangle(sf::Vector2f pos, float d, sf::Color color) {
	add_circle(pos, d, color, 3);
}

void Visualization::add_diamond(sf::Vector2f pos, float d, sf::Color color) {
	add_circle(pos, d, color, 4);
}

void Visualization::add_rect(sf::Vector2f pos, sf::Vector2f sz, sf::Color color) {
	auto shape = std::make_unique<sf::RectangleShape>(sz);
	shape->setOrigin(sz / 2.0f);
	shape->setFillColor(color);
	shape->setPosition(pos);
	next_shapes.push_back(std::move(shape));
}

void Visualization::add_line(sf::Vector2f p1, sf::Vector2f p2,
	   	sf::Color color, float thickness) {
	auto d = p1 - p2;
	sf::Vector2f sz(sqrt(d.x * d.x + d.y * d.y), thickness);
	auto shape = std::make_unique<sf::RectangleShape>(sz);
	shape->setOrigin(sz / 2.0f);
	shape->setFillColor(color);
	shape->setPosition((p1 + p2) / 2.0f);
	shape->rotate(180 / M_PI * atan2(d.y, d.x));
	next_shapes.push_back(std::move(shape));
}

void Visualization::add_shape(std::unique_ptr<sf::Shape>&& shape) {
	next_shapes.push_back(std::move(shape));
}

void Visualization::flip() {
	std::lock_guard<std::mutex> lock(mtx);
	this->shapes = std::move(next_shapes);
	next_shapes.clear();
}
