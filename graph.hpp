#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <cfloat>

using dtype = double;

class Graph {
    public:
    sf::Color background = sf::Color::Black;
    sf::Font font;
    float fontsize = 20;
    std::vector<std::vector<dtype>> x_valuess, y_valuess;
    std::vector<std::vector<dtype>> x_valuess_p, y_valuess_p;
    std::vector<sf::Color> colors, colors_p;
    std::vector<dtype> rads;
    dtype x_c, x_k;
    dtype y_c, y_k;
    sf::RenderWindow *window = nullptr;
    int window_xsize = 1080, window_ysize = 720;
    int fps = 30;
    int axis_thickness = 1, axis_mark_thickness = 1, axis_mark_length = 19;
    int text_xshift = 4, text_yshift = 23;
    int label_spacing = 100;
    dtype drag_start_x, drag_start_y;
    bool drag = false;
    dtype scale_k = 1.1;
    dtype big_scale_k = 4;
    bool plecement_unset = true;
    sf::Color axes_color = sf::Color::White;
    sf::String label = "Graph";
    
    Graph(sf::String label = "Graph", int xsize = 1080, int ysize = 720) : window_xsize(xsize), window_ysize(ysize), label(label) {
        x_k = (dtype)10 / window_xsize;
        y_k = (dtype)10 / window_ysize;
        x_c = y_c = -5;
        font.loadFromFile("CyrilicOld.TTF");
    }

    Graph(const Graph& other) {
        *this = other;
    }

    Graph& operator=(const Graph& other) {
        font = other.font;
        background = other.background;
        x_valuess = other.x_valuess; y_valuess = other.y_valuess;
        x_valuess_p = other.x_valuess_p; y_valuess_p = other.y_valuess_p;
        colors = other.colors; colors_p = other.colors_p;
        rads = other.rads;
        x_c = other.x_c; x_k = other.x_k;
        y_c = other.y_c; y_k = other.y_k;
        window_xsize = other.window_xsize;
        window_ysize = other.window_ysize;
        if(window != nullptr) delete window;
        window = nullptr;
        fps = other.fps;
        axis_thickness = other.axis_thickness; axis_mark_thickness = other.axis_mark_thickness;
        axis_mark_length = other.axis_mark_length;
        text_xshift = other.text_xshift; text_yshift = other.text_yshift;
        label_spacing = other.label_spacing;
        scale_k = other.scale_k; big_scale_k = other.big_scale_k;
        axes_color = other.axes_color;
        label = other.label;
        fontsize = other.fontsize;
        plecement_unset = other.plecement_unset;
        drag = false;
        return *this;
    }

    void graph(std::vector<dtype> x_values, std::vector<dtype> y_values, sf::Color color = sf::Color::White) {
        if(!x_values.size() || !y_values.size())
            throw std::runtime_error("Graph::graph: empty input");
        if(x_values.size() != y_values.size())
            throw std::runtime_error("Graph::graph: x_values and y_values do not match");
        x_valuess.push_back(x_values);
        y_valuess.push_back(y_values);
        colors.push_back(color);
    }

    void graph_points(std::vector<dtype> x_values, std::vector<dtype> y_values, sf::Color color = sf::Color::White, dtype rad = 5) {
        if(!x_values.size() || !y_values.size())
            throw std::runtime_error("Graph::graph_points: empty input");
        if(x_values.size() != y_values.size())
            throw std::runtime_error("Graph::graph_points: x_values and y_values do not match");
        x_valuess_p.push_back(x_values);
        y_valuess_p.push_back(y_values);
        colors_p.push_back(color);
        rads.push_back(rad);
    }

    void pop() {
        if(size()) {
            x_valuess.pop_back();
            y_valuess.pop_back();
            colors.pop_back();
        }
    }

    void pop_points() {
        if(size_points()) {
            x_valuess_p.pop_back();
            y_valuess_p.pop_back();
            colors_p.pop_back();
            rads.pop_back();
        }
    }

    int size() { return colors.size(); }

    int size_points() { return rads.size(); }

    void reset_plecement(bool fixed_ratio = false) {
        plecement_unset = false;
        if(!size() and !size_points()) {
            x_k = (dtype)10 / window_xsize;
            y_k = (dtype)10 / window_ysize;
            x_c = y_c = -5;
            return;
        }
        dtype x_min = DBL_MAX, x_max = -DBL_MAX, y_min = DBL_MAX, y_max = -DBL_MAX;
        for(int i = 0; i < size(); i ++) {
            x_min = std::min(*std::min_element(x_valuess[i].begin(), x_valuess[i].end()), x_min);
            x_max = std::max(*std::max_element(x_valuess[i].begin(), x_valuess[i].end()), x_max);
            y_min = std::min(*std::min_element(y_valuess[i].begin(), y_valuess[i].end()), y_min);
            y_max = std::max(*std::max_element(y_valuess[i].begin(), y_valuess[i].end()), y_max);
        }
        for(int i = 0; i < size_points(); i ++) {
            x_min = std::min(*std::min_element(x_valuess_p[i].begin(), x_valuess_p[i].end()), x_min);
            x_max = std::max(*std::max_element(x_valuess_p[i].begin(), x_valuess_p[i].end()), x_max);
            y_min = std::min(*std::min_element(y_valuess_p[i].begin(), y_valuess_p[i].end()), y_min);
            y_max = std::max(*std::max_element(y_valuess_p[i].begin(), y_valuess_p[i].end()), y_max);
        }
        reset_plecement(x_min, x_max, y_min, y_max, fixed_ratio);
    }

    void reset_plecement(dtype x_min, dtype x_max, dtype y_min, dtype y_max, bool fixed_ratio = false) {
        plecement_unset = false;
        x_k = (window_xsize - 1) / (x_max - x_min);
        x_c = -x_min * x_k;
        if(x_max == x_min) {
            x_k = window_xsize;
            x_c = window_xsize / 2 - x_max * x_k;
        }
        y_k = (window_ysize - 1) / (y_max - y_min);
        y_c = -y_min * y_k;
        if(y_max == y_min) {
            y_k = window_ysize;
            y_c = window_ysize / 2 - y_max * y_k;
        }
        if(fixed_ratio) {
            dtype k = std::min(x_k, y_k);
            x_c = (1 - k / x_k) * window_xsize * .5 + x_c * k / x_k;
            x_k = k;
            y_c = (1 - k / y_k) * window_ysize * .5 + y_c * k / y_k;
            y_k = k;
        }
    }

    void close() {
        if(window != nullptr) {
            delete window;
            window = nullptr;
        }
        plecement_unset = true;
    }

    bool show(float duration = 0.0f) {
        if(window == nullptr) {
            _open_window();
            if(plecement_unset) reset_plecement();
        }
        bool running = true;
        sf::Clock clock;
        while(running && (duration <= 0 || clock.getElapsedTime().asSeconds() < duration)) {
            draw();
            window->display();
            sf::Event event;
            while(running && window->pollEvent(event)) {
                dtype xk, yk;
                switch(event.type) {
                    case sf::Event::Closed:
                        close();
                        running = false;
                        break;
                    case sf::Event::Resized:
                        process_resize(event.size.width, event.size.height);
                        break;
                    case sf::Event::MouseWheelScrolled:
                        xk = pow(sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) ? big_scale_k : scale_k, event.mouseWheelScroll.delta);
                        yk = xk;
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::RControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                            xk = 1;
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                            yk = 1;
                        process_scale(event.mouseWheelScroll.x,
                                      window_ysize - event.mouseWheelScroll.y,
                                      xk, yk);
                        if(drag) {
                            x_c = (event.mouseWheelScroll.x / x_k - drag_start_x) * x_k;
                            y_c = ((window_ysize - 1 - event.mouseWheelScroll.y) / y_k - drag_start_y) * y_k;
                        }
                        break;
                    case sf::Event::KeyPressed:
                        if(event.key.code == sf::Keyboard::RAlt || event.key.code == sf::Keyboard::LAlt)
                            process_snap(sf::Mouse::getPosition(*window).x,
                                         window_ysize - sf::Mouse::getPosition(*window).y);
                        if(event.key.code == sf::Keyboard::Home)
                            reset_plecement();
                        if(event.key.code == sf::Keyboard::O) {
                            x_c = sf::Mouse::getPosition(*window).x;
                            y_c = window_ysize - 1 - sf::Mouse::getPosition(*window).y;
                        }
                        break;
                    case sf::Event::MouseButtonPressed:
                        if(event.mouseButton.button == sf::Mouse::Left) {
                            drag_start_x = (event.mouseButton.x - x_c) / x_k;
                            drag_start_y = (window_ysize - 1 - event.mouseButton.y - y_c) / y_k;
                            drag = true;
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
                        if(event.mouseButton.button == sf::Mouse::Left) drag = false;
                        break;
                    case sf::Event::MouseMoved:
                        if(drag) {
                            sf::Vector2i pos = sf::Mouse::getPosition(*window);
                            int x = pos.x;
                            int y = pos.y;
                            if(x < 0 || x >= window_xsize) {
                                int delta = ((x % window_xsize) + window_xsize) % window_xsize - x;
                                drag_start_x += delta / x_k;
                                x += delta;
                                sf::Mouse::setPosition(sf::Vector2i(x, y), *window);
                            }
                            if(y < 0 || y >= window_ysize) {
                                int delta = ((y % window_ysize) + window_ysize) % window_ysize - y;
                                drag_start_y -= delta / y_k;
                                y += delta;
                                sf::Mouse::setPosition(sf::Vector2i(x, y), *window);
                            }
                            x_c = (x / x_k - drag_start_x) * x_k;
                            y_c = ((window_ysize - 1 - y) / y_k - drag_start_y) * y_k;
                        }
                        break;
                }
            }
            if(duration == 0.0f) break;
        }
        return running;
    }

    void draw() {
        window->clear(background);
        window->setTitle(label);
        window->setFramerateLimit(fps);
        int xline = floor(y_c + .5);
        if(xline < 0 || xline >= window_ysize) xline = axis_thickness - axis_thickness / 2 - 1;
        int yline = floor(x_c + .5);
        if(yline < 0 || yline >= window_xsize) yline = axis_thickness / 2;
        sf::RectangleShape Xline(sf::Vector2f(window_xsize, axis_thickness));
        Xline.setPosition(sf::Vector2f(0, window_ysize - 1 - xline - axis_thickness / 2));
        Xline.setFillColor(axes_color);
        sf::RectangleShape Yline(sf::Vector2f(axis_thickness, window_ysize));
        Yline.setPosition(sf::Vector2f(yline - axis_thickness / 2, 0));
        Yline.setFillColor(axes_color);
        window->draw(Xline);
        window->draw(Yline);
        dtype xstep = get_step(x_k);
        dtype ystep = get_step(y_k);
        for(dtype xlabel = -floor(x_c / x_k / xstep + .5) * xstep; xlabel <= (floor((-x_c + window_xsize - 1)  / x_k / xstep) + .5) * xstep; xlabel += xstep) {
            if(std::abs(xlabel) < xstep / 2) xlabel = 0;
            std::string s = std::to_string(xlabel);
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            s.erase(s.find_last_not_of('.') + 1, std::string::npos);
            sf::Text Xlabel(s, font, fontsize);
            Xlabel.setFillColor(axes_color);
            Xlabel.setPosition(sf::Vector2f(xlabel * x_k + x_c + text_xshift, window_ysize - 1 - xline - text_yshift));
            window->draw(Xlabel);
            if(xlabel == 0) continue;
            sf::RectangleShape mark(sf::Vector2f(axis_mark_thickness, axis_mark_length));
            mark.setPosition(sf::Vector2f(xlabel * x_k + x_c - axis_mark_thickness / 2, window_ysize - xline - 1 - axis_mark_length / 2));
            mark.setFillColor(axes_color);
            window->draw(mark);
        }
        for(dtype ylabel = -floor(y_c / y_k / ystep + .5) * ystep; ylabel <= (floor((-y_c + window_ysize - 1)  / y_k / ystep) + .5) * ystep; ylabel += ystep) {
            if(std::abs(ylabel) < ystep / 2) ylabel = 0;
            std::string s = std::to_string(ylabel);
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            s.erase(s.find_last_not_of('.') + 1, std::string::npos);
            sf::Text Ylabel(s, font, fontsize);
            Ylabel.setFillColor(axes_color);
            Ylabel.setPosition(sf::Vector2f(yline + text_xshift, window_ysize - 1 - ylabel * y_k - y_c - text_yshift));
            window->draw(Ylabel);
            if(ylabel == 0) continue;
            sf::RectangleShape mark(sf::Vector2f(axis_mark_length, axis_mark_thickness));
            mark.setPosition(sf::Vector2f(yline - axis_mark_length / 2, window_ysize - 1 - ylabel * y_k - y_c - axis_mark_thickness / 2));
            mark.setFillColor(axes_color);
            window->draw(mark);
        }
        dtype y_step = pow(2, floor(log2(200 / y_k)));
        for(int i = 0; i < size(); i ++) {
            std::vector<dtype> x_values = x_valuess[i];
            std::vector<dtype> y_values = y_valuess[i];
            sf::Color color = colors[i];
            for(int j = 1; j < x_values.size(); j ++) {
                sf::Vertex line[2];
                dtype x1 = x_c + x_values[j - 1] * x_k + .5;
                dtype y1 = window_ysize - .5 - (y_c + y_values[j - 1] * y_k);
                dtype x2 = x_c + x_values[j] * x_k + .5;
                dtype y2 = window_ysize - .5 - (y_c + y_values[j] * y_k);
                line[0] = sf::Vertex(sf::Vector2f(x1, y1), color);
                line[1] = sf::Vertex(sf::Vector2f(x2, y2), color);
                window->draw(line, 2, sf::Lines);
            }
        }
        for(int i = 0; i < size_points(); i++) {
            std::vector<dtype> x_values = x_valuess_p[i];
            std::vector<dtype> y_values = y_valuess_p[i];
            sf::Color color = colors_p[i];
            dtype rad = rads[i];
            for (int j = 0; j < x_values.size(); j++) {
                sf::CircleShape c;
                c.setFillColor(color);
                c.setRadius(rad);
                c.setOrigin(rad, rad);
                dtype x = x_c + x_values[j] * x_k + .5;
                dtype y = window_ysize - 1 - (y_c + y_values[j] * y_k) + .5;
                c.setPosition(x, y);
                window->draw(c);
            }
        }
    }

    void process_resize(int new_xsize, int new_ysize) {
        x_k = x_k / window_xsize * new_xsize;
        x_c = x_c / window_xsize * new_xsize;
        y_k = y_k / window_ysize * new_ysize;
        y_c = y_c / window_ysize * new_ysize;
        window_xsize = new_xsize;
        window_ysize = new_ysize;
        window->setView(sf::View(sf::FloatRect(0, 0, new_xsize, new_ysize)));
    }

    void process_snap(int x, int y) {
        dtype k = (x_k + y_k) / 2;
        x_c = x - k * (x - x_c) / x_k;
        y_c = y - k * (y - y_c) / y_k;
        x_k = y_k = k;
    }

    void process_scale(int x, int y, dtype xk, dtype yk) {
        x_c = x_c * xk + x * (1 - xk);
        y_c = y_c * yk + y * (1 - yk);
        x_k *= xk;
        y_k *= yk;
    }

    void clear() {
        x_valuess.clear();
        y_valuess.clear();
        colors.clear();
        x_valuess_p.clear();
        y_valuess_p.clear();
        colors_p.clear();
        rads.clear();
    }

    ~Graph() {
        if(window != nullptr)
            delete window;
    }

    protected:
    void _open_window() {
        window = new sf::RenderWindow(sf::VideoMode(window_xsize, window_ysize), label);
    }

    dtype get_step(dtype k) {
        dtype step = 1;
        while(step * k < label_spacing) {
            step *= 10;
        }
        while(step * k >= label_spacing * 2) {
            step /= 2; if(step * k < label_spacing * 2.5) break;
            step /= 2.5; if(step * k < label_spacing * 2) break;
            step /= 2;
        }
        return step;
    }
};
