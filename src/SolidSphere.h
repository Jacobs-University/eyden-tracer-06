#pragma once

#include "SolidQuad.h"

// ================================ Sphere Solid Class ================================
/**
* @brief Cone Solid class
* @ingroup modulePrimitive
*/
class CSolidSphere : public CSolid
{
     // Returns normalized normal vector
     Vec3f calcNormal(float phi, float theta) {
         return Vec3f(cosf(theta) * cosf(phi), sinf(theta), cosf(theta) * sinf(phi));
     }

public:
    /**
    * @brief Constructor
    * @param pShader Pointer to the shader
    * @param origin The origin of the sphere
    * @param radius The radius of the sphere
    * @param sides The number of sides
    * @param smooth Flag indicating whether the normals should be smoothed
    */
    CSolidSphere(ptr_shader_t pShader, const Vec3f& origin = Vec3f::all(0), float radius = 1, size_t sides = 24, bool smooth = true) : CSolid(origin)
    {
        size_t height_segments = sides / 2;
        float t0 = 0;								            // Initial texture coordinate
        float phi0 = 0;
        for (size_t s = 0; s < sides; s++) {
            float t1 = static_cast<float>(s + 1) / sides;       // Next texture coordinate: [1/sides; 1]
            float phi1 = -2 * Pif * t1;

            float h0 = 0.0f / height_segments;					// Initial height
            float theta0 = Pif * (h0 - 0.5f);
            Vec3f n00 = calcNormal(phi0, theta0);
            Vec3f n10 = calcNormal(phi1, theta0);
            for (size_t h = 0; h < height_segments; h++) {
                float h1 = static_cast<float>(h + 1) / height_segments;		// Next height: [1/height_segments; 1]
                float theta1 = Pif * (h1 - 0.5f);
                Vec3f n01 = calcNormal(phi0, theta1);
                Vec3f n11 = calcNormal(phi1, theta1);

                if (h == 0) { // ----- Bottom cap: triangles -----
                    if (smooth)
                        add(std::make_shared<CPrimTriangle>(pShader,
                            origin + n00 * radius,
                            origin + n11 * radius,
                            origin + n01 * radius,
                            Vec2f(t0, 1 - h0), Vec2f(t1, 1 - h1), Vec2f(t0, 1 - h1),
                            n00, n11, n01));
                    else
                        add(std::make_shared<CPrimTriangle>(pShader,
                            origin + n00 * radius,
                            origin + n11 * radius,
                            origin + n01 * radius,
                            Vec2f(t0, 1 - h0), Vec2f(t1, 1 - h1), Vec2f(t0, 1 - h1)));
                }
                else if (h == height_segments - 1) { // ----- Top cap: triangles -----
                    if (smooth)
                        add(std::make_shared<CPrimTriangle>(pShader,
                            origin + n00 * radius,
                            origin + n10 * radius,
                            origin + n11 * radius,
                            Vec2f(t0, 1 - h0), Vec2f(t1, 1 - h0), Vec2f(t1, 1 - h1),
                            n00, n10, n11));
                    else
                        add(std::make_shared<CPrimTriangle>(pShader,
                            origin + n00 * radius,
                            origin + n10 * radius,
                            origin + n11 * radius,
                            Vec2f(t0, 1 - h0), Vec2f(t1, 1 - h0), Vec2f(t1, 1 - h1)));
                }
                else { // ----- Sides: quads -----
                    if (smooth)
                        add(std::make_shared<CSolidQuad>(pShader,
                            origin + n00 * radius,
                            origin + n10 * radius,
                            origin + n11 * radius,
                            origin + n01 * radius,
                            Vec2f(t0, 1 - h0), Vec2f(t1, 1 - h0), Vec2f(t1, 1 - h1), Vec2f(t0, 1 - h1),
                            n00, n10, n11, n01));
                    else
                        add(std::make_shared<CSolidQuad>(pShader,
                            origin + n00 * radius,
                            origin + n10 * radius,
                            origin + n11 * radius,
                            origin + n01 * radius,
                            Vec2f(t0, 1 - h0), Vec2f(t1, 1 - h0), Vec2f(t1, 1 - h1), Vec2f(t0, 1 - h1)));
                }
                h0 = h1;
                theta0 = theta1;
                n00 = n01;
                n10 = n11;
            } // h
            t0 = t1;
            phi0 = phi1;
        } // s
    }
    ~CSolidSphere() override = default;
};
