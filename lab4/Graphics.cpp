#include "StdAfx.h"
#include "GF.h"
#include <vector>

#ifndef M_PI
const double M_PI = 3.1415926535897932384626433832795;
#endif

using point_t = std::pair<double, double>;

// ���� ���������� ����������� ��� �������

void DrawLine(int x1, int y1, int x2, int y2, RGBPIXEL color); // ������ �������

void DrawPolygon(const std::vector<std::pair<int, int>>& vertices, RGBPIXEL color); // ������ �������

enum CLPointType { LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION }; // ������������� ��������� ����� ���-�� �������

CLPointType Classify(int x1, int y1, int x2, int y2, int x, int y); // ������� ��� ����������� ��������� ����� ���-�� �������

bool IsConvexPolygon(const std::vector<std::pair<int, int>>& points); // �������� �� �������

bool IsClockwise(const std::vector<std::pair<int, int>>& points); // ������������ �� ������� �� ������� �������

// ������� ��� ���������� ������������ N

double Dist(point_t p); 

double H(point_t p0, point_t p1, point_t p2, point_t p3);

// ������� ��� �������������� ���������� ������

point_t line(point_t p0, point_t p1, double t);

point_t BezierQuadratic(point_t p0, point_t p1, point_t p2, double t);

point_t BezierCubic(point_t p0, point_t p1, point_t p2, point_t p3, double t);

void BezierCurve(point_t p0, point_t p1, point_t p2, point_t p3); // ������� ��� ���������� ������ ����� �������� �������

// ����������� ����� �������, ������� � �������� �� ��������� ������-����

int CyrusBeckClipLine(double& x1, double& y1, double& x2, double& y2, const std::vector<std::pair<int, int>>& vertices, int n); 

void Clip(double& x1, double& y1, double& x2, double& y2, const std::vector<std::pair<int, int>>& vertices, int n);

// ���������� ���� ��� � ����� ������ ��� ������������� ����������
bool gfInitScene()
{
    gfSetWindowSize( 640, 480 );

    // ���������� ������ ����� �������� �������
    /*
    point_t p0 = { 10, 100 };
    point_t p1 = { 110, 100 };
    point_t p2 = { 210, 100 };
    point_t p3 = { 310, 100 };

    BezierCurve(p0, p1, p2, p3);
    */
    point_t p0 = { 20, 360 };
    point_t p1 = { 30, 240 };
    point_t p2 = { 100, 420 };
    point_t p3 = { 150, 370 };

    BezierCurve(p0, p1, p2, p3);
    
    // ��������� ������� �������� ���������

    std::vector<std::pair<int, int>> polygon = {
        {200, 150},
        {300, 150},
        {300, 50},
        {200, 50}
    };
    /*
    std::vector<std::pair<int, int>> polygon = {
        {200, 50},
        {300, 50},
        {300, 150},
        {200, 150}
    };

    std::vector<std::pair<int, int>> polygon = {
    {200, 50},   // A
    {150, 400},  // B
    {100, 200},  // C
    {300, 100},  // D
    {350, 250},  // E
    {50, 350},   // F
    {250, 200}   // G
    };
    */
    std::vector<std::pair<double, double>> line = {
        {100, 50},
        {400, 150}
    };

    Clip(line[0].first, line[0].second, line[1].first, line[1].second, polygon, polygon.size());
    
    return true;
}

// ���������� � ����� �� ������� ������ �� ����������.
// ������� ������������ ��� �������� ������������ ��������
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

// ���������� ���� ��� ����� ������� �� ����������.
// ������� ������������ ��� ������������ ����������
// �������� (������, ������ � �.�.)
void gfCleanupScene()
{
}

// ���������� ����� ������������ �������� ����� ������ ����
void gfOnLMouseClick( int x, int y )
{
    x; y;
    gfDrawRectangle(x - 10, y - 10, x + 10, y + 10, RGBPIXEL::Green());
}

// ���������� ����� ������������ �������� ������ ������ ����
void gfOnRMouseClick( int x, int y )
{
    x; y;
}

// ���������� ����� ������������ �������� ������� �� ����������
void gfOnKeyDown( UINT key )
{
    key;

    if( key == 'A' )
        gfDisplayMessage( "'A' key has been pressed" );
}

// ���������� ����� ������������ �������� ������� �� ����������
void gfOnKeyUp( UINT key )
{
    key;

    //if( key == 'B' )
    //    gfDisplayMessage( "'B' key has been un-pressed" );
}

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
    if (n < 3) return; // ������ ��������, ���� ������ 3 ������

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
    if ((ax * bx < 0) || (ay * by < 0)) //�������������� �����������
        return BEHIND; //������
    if ((ax * ax + ay * ay) < (bx * bx + by * by))
        return BEHIND; //�������
    if (x1 == x && y1 == y) //��������� � �������
        return ORIGIN;
    if (x2 == x && y2 == y) //��������� � ������
        return DESTINATION;
    return BETWEEN; //�����
}

bool IsConvexPolygon(const std::vector<std::pair<int, int>>& points) {
    if (points.size() < 3) return false; // ������� ������ ����� ������� 3 �������

    int n = points.size();
    CLPointType globalSide = ORIGIN; // ��������� ��������� (��� �����������)

    // ��������� ��� ������� ��������
    for (int i = 0; i < n; i++) {
        // ������� ������� �������
        int x1 = points[i].first;
        int y1 = points[i].second;
        int x2 = points[(i + 1) % n].first;
        int y2 = points[(i + 1) % n].second;

        CLPointType localSide = ORIGIN; // ����������� ��� ������� �������

        // ��������� ��� ��������� �������
        for (int j = 0; j < n; j++) {
            if (j == i || j == (i + 1) % n) continue; // ���������� ������� �������

            int x = points[j].first;
            int y = points[j].second;

            CLPointType type = Classify(x1, y1, x2, y2, x, y);

            // ��������� ������ ��������� LEFT � RIGHT
            if (type == LEFT || type == RIGHT) {
                if (localSide == ORIGIN) {
                    localSide = type; // ������������� �����������
                }
                else if (localSide != type) {
                    return false; // ������� ��������� � ������ ������
                }
            }
        }

        // ������������� ���������� �����������
        if (globalSide == ORIGIN) {
            globalSide = localSide;
        }
        else if (globalSide != localSide) {
            return false; // ����������� ���������� ��� ������ �������
        }
    }

    return true; // ������� ��������
}

bool IsClockwise(const std::vector<std::pair<int, int>>& points) {
    if (points.size() < 3) return false; // ������� ������ ����� ������� 3 �������

    int n = points.size();
    int left = 0;
    int right = 0;
    CLPointType globalSide = ORIGIN; // ��������� ��������� (��� �����������)

    // ��������� ��� ������� ��������
    for (int i = 0; i < n; i++) {
        // ������� ������� �������
        int x1 = points[i].first;
        int y1 = points[i].second;
        int x2 = points[(i + 1) % n].first;
        int y2 = points[(i + 1) % n].second;

        CLPointType localSide = ORIGIN; // ����������� ��� ������� �������

        // ��������� ��� ��������� �������
        for (int j = 0; j < n; j++) {
            if (j == i || j == (i + 1) % n) continue; // ���������� ������� �������

            int x = points[j].first;
            int y = points[j].second;

            CLPointType type = Classify(x1, y1, x2, y2, x, y);

            // ��������� ������ ��������� LEFT � RIGHT
            if (type == LEFT) {
                left++;
            }
            if (type == RIGHT) {
                right++;
            }
        }
    }

    return right > left;
}

double Dist(point_t p)
{
    return abs(p.first) + abs(p.second);
}

double H(point_t p0, point_t p1, point_t p2, point_t p3)
{
    point_t h1 = { p0.first - 2 * p1.first + p2.first, p0.second - 2 * p1.second + p2.second };
    point_t h2 = { p1.first - 2 * p2.first + p3.first, p1.second - 2 * p2.second + p3.second };
    return max(Dist(h1), Dist(h2));
}

point_t line(point_t p0, point_t p1, double t)
{
    return { p0.first * (1.0 - t) + p1.first * t, p0.second * (1.0 - t) + p1.second * t };
}

point_t BezierQuadratic(point_t p0, point_t p1, point_t p2, double t)
{
    return line(line(p0, p1, t), line(p1, p2, t), t);
}

point_t BezierCubic(point_t p0, point_t p1, point_t p2, point_t p3, double t)
{
    return line(BezierQuadratic(p0, p1, p2, t), BezierQuadratic(p1, p2, p3, t), t);
}

void BezierCurve(point_t p0, point_t p1, point_t p2, point_t p3)
{
    DrawLine(p0.first, p0.second, p1.first, p1.second, RGBPIXEL::DkYellow());
    DrawLine(p1.first, p1.second, p2.first, p2.second, RGBPIXEL::DkYellow());
    DrawLine(p2.first, p2.second, p3.first, p3.second, RGBPIXEL::DkYellow());

    int N = 1 + sqrt(3 * H(p0, p1, p2, p3));

    point_t prev_p = p0;

    for (double t = 0; t < 1; t += 1. / N)
    {
        point_t cur_p = BezierCubic(p0, p1, p2, p3, t);

        DrawLine(prev_p.first, prev_p.second, cur_p.first, cur_p.second, RGBPIXEL::Blue());

        prev_p = cur_p;
    }

    DrawLine(prev_p.first, prev_p.second, p3.first, p3.second, RGBPIXEL::Blue());
}

// ���������� 0, ����� ������� ��� ��������; 1, ����� ���� �� ����� ������� ������
// ������� ������������ �� ������� �������
int CyrusBeckClipLine(double& x1, double& y1, double& x2, double& y2, const std::vector<std::pair<int, int>>& vertices, int n) {
    double t1 = 0, t2 = 1, t;
    double sx = x2 - x1, sy = y2 - y1;
    double nx, ny, denom, num, x1_new, y1_new, x2_new, y2_new;
    for (int i = 0; i < n; i++) {
        nx = vertices[(i + 1) % n].second - vertices[i].second; ny = vertices[i].first - vertices[(i + 1) % n].first; // ������� � ������� ��������
        denom = nx * sx + ny * sy; //���������� ����� �����������
        num = nx * (x1 - vertices[i].first) + ny * (y1 - vertices[i].second);
        if (denom != 0) { //����� �� �����������
            t = -num / denom;
            if (denom > 0) { //����� t ������������ �������� (��)
                if (t > t1) t1 = t;
            }
            else { //������������ ���������� (��)

                if (t < t2) t2 = t;
            }

        }
        else { if (Classify(vertices[i].first, vertices[i].second, vertices[(i + 1) % n].first, vertices[(i + 1) % n].second, x1, y1) == LEFT) return 0; } // �����������
    } //for
    if (t1 <= t2) { //���������� ������� �������
        x1_new = x1 + t1 * (x2 - x1); y1_new = y1 + t1 * (y2 - y1);
        x2_new = x1 + t2 * (x2 - x1); y2_new = y1 + t2 * (y2 - y1);
        x1 = x1_new; y1 = y1_new; x2 = x2_new; y2 = y2_new;
        return 1;
    }
    return 0;
}

void Clip(double& x1, double& y1, double& x2, double& y2, const std::vector<std::pair<int, int>>& vertices, int n) {
    if (!IsConvexPolygon(vertices)) {
        gfDisplayMessage("������� ����������!");
    }
    if (!IsClockwise(vertices)) {
        gfDisplayMessage("������� ������������ ������ ������� �������, �������� �� ����� �������� ���������!");
    }

    DrawPolygon(vertices, RGBPIXEL::Green());

    DrawLine(x1, y1, x2, y2, RGBPIXEL::Cyan());

    int status = CyrusBeckClipLine(x1, y1, x2, y2, vertices, n);
    if (status == 0) return;
    DrawLine(x1, y1, x2, y2, RGBPIXEL::Red());
}