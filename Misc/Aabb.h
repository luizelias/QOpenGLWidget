#ifndef AABB_H
#define AABB_H

#include "Types.h"

class Aabb
{
public:
    Aabb();
    ~Aabb();

    vec3 getCenter() const;

    void expand(const vec4 &point);
    void expand(const vec3 &point);
    void expand(const Aabb &aabb);
    void setScale(float s);

    float getWidth();
    float getHeight();
    float getDepth();

    vec3 getMinPoint();
    vec3 getMaxPoint();

    bool isFlatt();

    void draw(const mat4 & mvp);

public :
    vec3 _min;
    vec3 _max;
    bool _initialized;

};
#endif // AABB_H
