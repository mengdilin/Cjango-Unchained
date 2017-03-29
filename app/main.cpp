#include "app.hpp"

int main(int argc, char* argv[])
{
    URLmap um;
    App app(um);
    app.run(8080);

    return 0;
}