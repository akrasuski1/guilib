#include <SFML/Graphics.hpp>

#include <thread>
#include <vector>
#include <mutex>


class Visualization {
public:
	Visualization(size_t w, size_t h);
	~Visualization();

	void set_event_handler(std::function<void(sf::Event)> fun);
	void set_command_handler(std::function<void(std::string)> fun);

	sf::Vector2f get_mouse_pos();

	void add_circle(sf::Vector2f pos, float d, sf::Color color, size_t edges = 20);
	void add_triangle(sf::Vector2f pos, float d, sf::Color color);
	void add_diamond(sf::Vector2f pos, float d, sf::Color color);
	void add_rect(sf::Vector2f pos, sf::Vector2f sz, sf::Color color);
	void add_line(sf::Vector2f p1, sf::Vector2f p2, sf::Color color,
		   	float thickness = 0.1);
	void add_shape(std::unique_ptr<sf::Shape>&& shape);
	void flip();

private:
	void draw();
	void loop();
	void rescale();
	void keyboard(sf::Event::KeyEvent key);

	sf::RenderWindow window;
	bool is_running;
	std::thread drawing_thread;
	std::vector<std::unique_ptr<sf::Shape>> shapes, next_shapes;
	std::mutex mtx;
	std::string command;
	bool console_mode;
	float scale;
	sf::Vector2f drag_start;
	sf::Font font;

	sf::View view;

	std::function<void(sf::Event)> handle_event;
	std::function<void(std::string)> handle_command;
};
