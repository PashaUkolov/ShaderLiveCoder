#include <iostream>
#include "App.h"

static const int WIDTH = 680;
static const int HEIGHT = 500;

int main() {
	App app;
	app.init(WIDTH, HEIGHT);
	app.run();

	return 0;
}
