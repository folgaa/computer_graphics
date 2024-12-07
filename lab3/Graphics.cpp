#include "StdAfx.h"
#include "GF.h"
#include <vector>

#ifndef M_PI
const double M_PI = 3.1415926535897932384626433832795;
#endif


// Ваша реализация необходимых вам функций
void DrawLine(int x1, int y1, int x2, int y2, RGBPIXEL color); // рисует отрезок

void DrawPolygon(const std::vector<std::pair<int, int>>& vertices, RGBPIXEL color); // рисует полигон

enum CLPointType { LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION }; // классификация положения точки отн-но отрезка

CLPointType Classify(int x1, int y1, int x2, int y2, int x, int y); // функция для определения положения точки отн-но отрезка

bool IsConvexPolygon(const std::vector<std::pair<int, int>>& points); // выпуклый ли полигон

enum IntersectType { SAME, PARALLEL, SKEW, SKEW_CROSS, SKEW_NO_CROSS }; // классификация расположения двух отрезков

IntersectType Intersect(int ax, int ay, int bx, int by, int cx, int cy,
    int dx, int dy, double* t); // функция для определения расположения двух отрезков

IntersectType Cross(int ax, int ay, int bx, int by, int cx, int cy,
    int dx, int dy, double* tab, double* tcd); // функция для определения наличия пересечения двух отрезков

bool IsSimplePolygon(const std::vector<std::pair<int, int>>& vertices); // является ли полигон простым

enum EType { TOUCHING, CROSS_LEFT, CROSS_RIGHT, INESSENTIAL }; // классификация стороны полигона отн-но луча

EType EdgeType(int ox, int oy, int dx, int dy, int ax, int ay); // функция для определения положения луча отн-но стороны полигона

enum PType { INSIDE, OUTSIDE }; // классификация точек отн-но полигона

PType PInPolygonEOMode(int x, int y,
    const std::vector<std::pair<int, int>>& vertices,
    int n); // определение принадлежности точки полигону по правилу Even-Odd

void FillPolygonEvenOdd(const std::vector<std::pair<int, int>>& vertices, int n, RGBPIXEL color); // закраска полигона

PType PInPolygonNZWMode(int x, int y,
    const std::vector<std::pair<int, int>>& vertices,
    int n); // определение принадлежности точки полигону по правилу Non-Zero Winding

void FillPolygonNonZeroWinding(const std::vector<std::pair<int, int>>& vertices, int n, RGBPIXEL color); // закраска полигона

// Вызывается один раз в самом начале при инициализации приложения
bool gfInitScene()
{
    gfSetWindowSize( 640, 480 );
    

    /*std::vector<std::pair<int, int>> polygon = {
    {200, 50},   // A
    {150, 400},  // B
    {100, 200},  // C
    {300, 100},  // D
    {350, 250},  // E
    {50, 350},   // F
    {250, 200}   // G
    };*/

    std::vector<std::pair<int, int>> polygon = {
    {200, 50},   // A
    {300, 50},  // B
    {300, 150},  // C
    {200, 150},  // D
    };


    DrawPolygon(polygon, RGBPIXEL::Yellow());

    // FillPolygonEvenOdd(polygon, 4, RGBPIXEL::Cyan());
    // FillPolygonNonZeroWinding(polygon, 4, RGBPIXEL::Cyan());

    const TCHAR* message = IsConvexPolygon(polygon) ? _T("Полигон выпуклый.") : _T("Полигон невыпуклый.");

    const TCHAR* message2 = IsSimplePolygon(polygon) ? _T("Полигон простой.") : _T("Полигон сложный.");

    gfDisplayMessage(message);

    gfDisplayMessage(message2);

    return true;
}

// Вызывается в цикле до момента выхода из приложения.
// Следует использовать для создания анимационных эффектов
void gfDrawScene()
{
    //gfClearScreen(RGBPIXEL::Black());

    //static int x = 0;
    //gfDrawRectangle(x, 100, x + 50, 130, RGBPIXEL::Blue());
    //x = (x + 1) % gfGetWindowWidth() ;

    //int x = gfGetMouseX(),
    //    y = gfGetMouseY();
    //gfDrawRectangle(x - 10, y - 10, x + 10, y + 10, RGBPIXEL::Green());
}

// Вызывается один раз перед выходом из приложения.
// Следует использовать для освобождения выделенных
// ресурсов (памяти, файлов и т.п.)
void gfCleanupScene()
{
}

// Вызывается когда пользователь нажимает левую кнопку мыши
void gfOnLMouseClick( int x, int y )
{
    x; y;
    gfDrawRectangle(x - 10, y - 10, x + 10, y + 10, RGBPIXEL::Green());
}

// Вызывается когда пользователь нажимает правую кнопку мыши
void gfOnRMouseClick( int x, int y )
{
    x; y;
}

// Вызывается когда пользователь нажимает клавишу на клавиатуре
void gfOnKeyDown( UINT key )
{
    key;

    if( key == 'A' )
        gfDisplayMessage( "'A' key has been pressed" );
}

// Вызывается когда пользователь отжимает клавишу на клавиатуре
void gfOnKeyUp( UINT key )
{
    key;

    //if( key == 'B' )
    //    gfDisplayMessage( "'B' key has been un-pressed" );
}

// Мои функции

void DrawLine(int x1, int y1, int x2, int y2, RGBPIXEL color)
{
    int x = x1, y = y1;
    int dx = x2 - x1, dy = y2 - y1;
    int ix, iy;
    int e;
    int i;

    if (dx > 0)
        ix = 1;
    else if (dx < 0)
    {
        ix = -1;
        dx = -dx;
    }
    else
        ix = 0;

    if (dy > 0)
        iy = 1;
    else if (dy < 0)
    {
        iy = -1;
        dy = -dy;
    }
    else
        iy = 0;

    if (dx >= dy)
    {
        e = 2 * dy - dx;
        for (i = 0; i <= dx; i++)
        {
            gfSetPixel(x, y, color);
            if (e >= 0)
            {
                y += iy;
                e -= 2 * dx;
            }
            x += ix;
            e += 2 * dy;
        }
    }
    else
    {
        e = 2 * dx - dy;
        for (i = 0; i <= dy; i++)
        {
            gfSetPixel(x, y, color);
            if (e >= 0)
            {
                x += ix;
                e -= 2 * dy;
            }
            y += iy;
            e += 2 * dx;
        }
    }

}

void DrawPolygon(const std::vector<std::pair<int, int>>& vertices, RGBPIXEL color)
{
    size_t n = vertices.size();
    if (n < 3) return; // нечего рисовать, если меньше 3 вершин

    for (size_t i = 0; i < n; ++i)
    {
        int x1 = vertices[i].first;
        int y1 = vertices[i].second;
        int x2 = vertices[(i + 1) % n].first;
        int y2 = vertices[(i + 1) % n].second;
        DrawLine(x1, y1, x2, y2, color);
    }
}

CLPointType Classify(int x1, int y1, int x2, int y2, int x, int y) {
    int ax = x2 - x1; //a
    int ay = y2 - y1;
    int bx = x - x1; //b
    int by = y - y1;
    int s = ax * by - bx * ay;
    if (s > 0) return LEFT;
    if (s < 0) return RIGHT;
    if ((ax * bx < 0) || (ay * by < 0)) //противоположно направлению
        return BEHIND; //позади
    if ((ax * ax + ay * ay) < (bx * bx + by * by))
        return BEHIND; //впереди
    if (x1 == x && y1 == y) //совпадает с началом
        return ORIGIN;
    if (x2 == x && y2 == y) //совпадает с концом
        return DESTINATION;
    return BETWEEN; //между
}

bool IsConvexPolygon(const std::vector<std::pair<int, int>>& points) {
    if (points.size() < 3) return false; // Полигон должен иметь минимум 3 вершины

    int n = points.size();
    CLPointType globalSide = ORIGIN; // Начальное положение (нет направления)

    // Проверяем все стороны полигона
    for (int i = 0; i < n; i++) {
        // Вершины текущей стороны
        int x1 = points[i].first;
        int y1 = points[i].second;
        int x2 = points[(i + 1) % n].first;
        int y2 = points[(i + 1) % n].second;

        CLPointType localSide = ORIGIN; // Направление для текущей стороны

        // Проверяем все остальные вершины
        for (int j = 0; j < n; j++) {
            if (j == i || j == (i + 1) % n) continue; // Пропускаем вершины стороны

            int x = points[j].first;
            int y = points[j].second;

            CLPointType type = Classify(x1, y1, x2, y2, x, y);

            // Учитываем только положения LEFT и RIGHT
            if (type == LEFT || type == RIGHT) {
                if (localSide == ORIGIN) {
                    localSide = type; // Устанавливаем направление
                }
                else if (localSide != type) {
                    return false; // Вершины оказались с разных сторон
                }
            }
        }

        // Устанавливаем глобальное направление
        if (globalSide == ORIGIN) {
            globalSide = localSide;
        }
        else if (globalSide != localSide) {
            return false; // Направление изменилось для другой стороны
        }
    }

    return true; // Полигон выпуклый
}

IntersectType Intersect(int ax, int ay, int bx, int by, int cx, int cy,
    int dx, int dy, double* t) {
    int nx = dy - cy; //вычисление координат n
    int ny = cx - dx;
    CLPointType type;
    double denom = nx * (bx - ax) + ny * (by - ay); //n ∗ b − a
    if (denom == 0) { //параллельны или совпадают
        type = Classify(cx, cy, dx, dy, ax, ay); // положение точки a относительно прямой cd
        if (type == LEFT || type == RIGHT)
            return PARALLEL;
        else return SAME;
    }
    double num = nx * (ax - cx) + ny * (ay - cy); //n ∗ a − c
    *t = -num / denom; // по значению t можно сделать вывод о пересечении отрезка ab
    return SKEW;
}

IntersectType Cross(int ax, int ay, int bx, int by, int cx, int cy,
    int dx, int dy, double* tab, double* tcd) {
    IntersectType type = Intersect(ax, ay, bx, by, cx, cy, dx, dy, tab);
    if (type == SAME || type == PARALLEL)
        return type;
    if ((*tab < 0) || (*tab > 1))
        return SKEW_NO_CROSS;
    Intersect(cx, cy, dx, dy, ax, ay, bx, by, tcd);
    if ((*tcd < 0) || (*tcd > 1))
        return SKEW_NO_CROSS;
    return SKEW_CROSS;
}

bool IsSimplePolygon(const std::vector<std::pair<int, int>>& vertices) {
    int n = vertices.size();

    for (int i = 0; i < n; ++i) {
        int ax = vertices[i].first, ay = vertices[i].second;
        int bx = vertices[(i + 1) % n].first, by = vertices[(i + 1) % n].second;

        for (int j = i + 2; j < n; ++j) {
            int cx = vertices[j].first, cy = vertices[j].second;
            int dx = vertices[(j + 1) % n].first, dy = vertices[(j + 1) % n].second;

            if (i == 0 && j == n - 1) continue;

            double tab, tcd;
            IntersectType type = Cross(ax, ay, bx, by, cx, cy, dx, dy, &tab, &tcd);

            if (type == SKEW_CROSS) {
                return false;
            }
        }
    }

    return true;
}

EType EdgeType(int ox, int oy, int dx, int dy, int ax, int ay) {
    switch (Classify(ox, oy, dx, dy, ax, ay)) {
    case LEFT:
        if (ay > oy && ay <= dy) return CROSS_LEFT; // пересекающая, A слева
        else return INESSENTIAL; // безразличная
    case RIGHT:
        if (ay > dy && ay <= oy) return CROSS_RIGHT; // пересекающая, A справа
        else return INESSENTIAL; // безразличная
    case BETWEEN:
    case ORIGIN:
    case DESTINATION:
        return TOUCHING; // касающаяся
    default:
        return INESSENTIAL; // безразличная
    }
}

PType PInPolygonEOMode(int x, int y, //точка
    const std::vector<std::pair<int, int>>& vertices, //координаты полигона
    int n) { //количество вершин в полигоне
    int param = 0;
    for (int i = 0; i < n; i++) {
        switch (EdgeType(vertices[i].first, vertices[i].second, vertices[(i + 1) % n].first, vertices[(i + 1) % n].second, x, y)) {
        case TOUCHING: //если лежит на полигоне, то заведомо принадлежит
            return INSIDE;
        case CROSS_LEFT:
        case CROSS_RIGHT:
            param = 1 - param; //изменяем значение четности
        }
    }
    if (param == 1) return INSIDE; //нечетное
    else return OUTSIDE;
}

void FillPolygonEvenOdd(const std::vector<std::pair<int, int>>& vertices, int n, RGBPIXEL color) {

    int max_x = 0, max_y = 0;
    int min_x = 640, min_y = 480;
    for (int i = 0; i < n; i++) {
        if (vertices[i].first > max_x) max_x = vertices[i].first;
        if (vertices[i].first < min_x) min_x = vertices[i].first;
        if (vertices[i].second > max_y) max_y = vertices[i].second;
        if (vertices[i].second < min_y) min_y = vertices[i].second;
    }

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (PInPolygonEOMode(x, y, vertices, n) == INSIDE) {
                gfSetPixel(x, y, color);
            }
        }
    }
}

PType PInPolygonNZWMode(int x, int y, //точка
    const std::vector<std::pair<int, int>>& vertices, //координаты полигона
    int n) { //количество вершин в полигоне
    int param = 0;
    for (int i = 0; i < n; i++) {
        switch (EdgeType(vertices[i].first, vertices[i].second, vertices[(i + 1) % n].first, vertices[(i + 1) % n].second, x, y)) {
        case TOUCHING: //если лежит на полигоне, то заведомо принадлежит
            return INSIDE;
        case CROSS_LEFT:
            param++;
            break;
        case CROSS_RIGHT:
            param--;
            break;
        }
    }
    if (param != 0) return INSIDE;
    else return OUTSIDE;
}

void FillPolygonNonZeroWinding(const std::vector<std::pair<int, int>>& vertices, int n, RGBPIXEL color) {

    int max_x = 0, max_y = 0;
    int min_x = 640, min_y = 480;
    for (int i = 0; i < n; i++) {
        if (vertices[i].first > max_x) max_x = vertices[i].first;
        if (vertices[i].first < min_x) min_x = vertices[i].first;
        if (vertices[i].second > max_y) max_y = vertices[i].second;
        if (vertices[i].second < min_y) min_y = vertices[i].second;
    }

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            if (PInPolygonNZWMode(x, y, vertices, n) == INSIDE) {
                gfSetPixel(x, y, color);
            }
        }
    }
}