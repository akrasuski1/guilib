#include "visualization.h"

int main() {
	Visualization visu(800, 600);

	int y = 0;
	visu.set_event_handler([&](auto event){
		if (event.type == sf::Event::KeyPressed) {
			y++; 
			printf("%d\n", y);
		}
	});
	visu.set_command_handler([&](auto str){
		printf("%s\n", str.c_str());
	});

	for (int i = 1; i < 10; i++) {
		for (int j = 0; j < 100; j++) {
			for (int k = 0; k < 100; k++) {
				visu.add_rect({float(j), float(k)}, {0.8, 0.8}, sf::Color::Yellow);
				visu.add_circle({float(j), float(k)}, 0.8f, sf::Color::Red);
			}
		}
		visu.add_circle({10, 10}, i * 4, sf::Color::Green);
		visu.add_triangle({50, 10}, 10, sf::Color::Magenta);
		visu.add_diamond({75, 10}, 10, sf::Color::Cyan);
		visu.add_rect({30, 40}, {15, 10}, sf::Color::Red);
		visu.add_line({10, 10}, {30, 40}, sf::Color::Blue);
		auto cs = std::make_unique<sf::CircleShape>(5);
		visu.add_shape(std::move(cs));

		visu.flip();

		sf::sleep(sf::milliseconds(300));
	}

	int x;
	scanf("%d", &x);
	printf("Work %d\n", x);
}
