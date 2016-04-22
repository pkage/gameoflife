#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <ctime>
#include <iostream>


using namespace std;


static int mapheight = 80, mapwidth = 80;
bool continuous = false;
vector< vector<bool> > inflate(int height, int width);
void applyRules(vector< vector<bool> > &primary, vector< vector<bool> > &cache);
int neighborcount(vector< vector<bool> > &primary, int y, int x);
void randomize(vector< vector<bool> > &arr);
void render(sf::RenderWindow &window, vector< vector<bool> > &arr);
void handleInput(sf::RenderWindow &window, vector< vector<bool> > &arr);

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
	sf::RectangleShape shape(sf::Vector2f(10,10));
	vector< vector<bool> > primary = inflate(mapheight, mapwidth), cache = inflate(mapheight, mapwidth);
	srand((int)time_t(0));

	randomize(primary);

	while (window.isOpen())
	{
		render(window, primary);

		// apply rules
		applyRules(primary, cache);
		cache.swap(primary);

		handleInput(window, primary);

	}

	return 0;
}

vector< vector<bool> > inflate(int height, int width) {
	vector< vector<bool> > arr;
	for (int y = 0; y < height; y++) {
		arr.push_back(vector<bool>(width, false));
	}
	return arr;
}



int neighborcount(vector< vector<bool> > &primary, int y, int x) {
	int count = 0;
	for (int sy = -1; sy <= 1; sy++) {
		for (int sx = -1; sx <= 1; sx++) {
			if (sx == 0 && sy == 0) continue;
			if (x + sx < 0 || x + sx >= mapwidth) continue;
			if (y + sy < 0 || y + sy >= mapheight) continue;
			if (primary[y + sy][x + sx]) count++;
		}
	}
	return count;
}


void applyRules(vector< vector<bool> > &primary, vector< vector<bool> > &cache) {
	int nc; // neighbor count
	for (int y = 0; y < mapheight; y++) {
		for (int x = 0; x < mapwidth; x++) {
			nc = neighborcount(primary, y, x);
			if (primary[y][x]) {
				if (nc < 2 || nc > 3) {
					// Any live cell with fewer than two live neighbours dies
					// Any live cell with more than three live neighbours dies
					cache[y][x] = false;
				} else if (nc == 2 || nc == 3) {
					// Any live cell with two or three live neighbours lives on to the next generation
					cache[y][x] = true;
				}
			} else if (nc == 3) {
				// Any dead cell with exactly three live neighbours becomes a live cell
				cache[y][x] = true;
			} else {
				// Nothing was here before, and nothing will be here now.
				cache[y][x] = false;
			}
		}
	}
}


void randomize(vector< vector<bool> > &arr) {
	for (int y = 0; y < mapheight; y++) {
		for (int x = 0; x < mapwidth; x++) {
			arr[y][x] = (rand()%100 == 0);
		}
	}
}
void render(sf::RenderWindow &window, vector< vector<bool> > &arr) {
	sf::RectangleShape shape(sf::Vector2f(10,10));
	window.clear();
	shape.setFillColor(sf::Color::Green);
	for (int y = 0; y < mapheight; y++) {
		for (int x = 0; x < mapwidth; x++) {
			if (arr[y][x]) {
				shape.setPosition(10 * y, 10 * x);
				window.draw(shape);
			}
		}
	}
	window.display();

}

void handleInput(sf::RenderWindow &window, vector< vector<bool> > &arr) {
	sf::Event event;
	if (continuous) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
			continuous = false;
		}
		window.pollEvent(event);
		if (event.type == sf::Event::Closed) {
			window.close();
		}
		sf::sleep(sf::milliseconds(100));
	} else {
		while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			window.pollEvent(event);
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::MouseButtonPressed) {
				// handle mouse interaction
				while (sf::Mouse::isButtonPressed(event.mouseButton.button)) {};
				sf::Vector2i mpos;
				mpos.x = event.mouseButton.x / 10; mpos.y = event.mouseButton.y / 10;
				arr[mpos.x][mpos.y] = !arr[mpos.x][mpos.y];
				render(window, arr);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
				randomize(arr);
				render(window, arr);
			} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
				while (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {};
				continuous = true;
				break;
			}
		};
	}

	// random!
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		randomize(arr);
		while (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {};
	}

	while (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
	};

	window.setTitle((continuous) ? "kage's GOL [continuous]" : "kage's GOL");

}
