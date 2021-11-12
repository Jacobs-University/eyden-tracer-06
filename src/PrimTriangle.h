// Triangle Geometrical Primitive class
// Written by Sergey Kosov in 2005 for Rendering Competition
#pragma once

#include "IPrim.h"
#include "Transform.h"
// ================================ Triangle Primitive Class ================================
/**
 * @brief Triangle Geometrical Primitive class
 */
class CPrimTriangle : public IPrim
{
public:
	/**
	 * @brief Constructor
	 * @param pShader Pointer to the shader to be applied for the prim
	 * @param a Position of the first vertex
	 * @param b Position of the second vertex
	 * @param c Position of the third vertex
 	 * @param ta Texture coordinate for the first vertex
 	 * @param tb Texture coordinate for the second vertex
 	 * @param tc Texture coordinate for the third vertex
 	 * @param na Normal at vertex a
 	 * @param nb Normal at vertex b
 	 * @param nc Normal at vertex c
	 */
	CPrimTriangle(ptr_shader_t pShader, 
		const Vec3f& a, const Vec3f& b, const Vec3f& c, 
		const Vec2f& ta = Vec2f::all(0), const Vec2f& tb = Vec2f::all(0), const Vec2f& tc = Vec2f::all(0), 
		std::optional<Vec3f> na = std::nullopt, std::optional<Vec3f> nb = std::nullopt, std::optional<Vec3f> nc = std::nullopt
	)
		: IPrim(pShader)
		, m_a(a)
		, m_b(b)
		, m_c(c)
		, m_ta(ta)
		, m_tb(tb)
		, m_tc(tc)
		, m_na(na)
		, m_nb(nb)
		, m_nc(nc)
		, m_edge1(b - a)
		, m_edge2(c - a)
	{}
	virtual ~CPrimTriangle(void) = default;

	virtual bool intersect(Ray& ray) const override
	{
		const Vec3f edge1 = m_b - m_a;
		const Vec3f edge2 = m_c - m_a;

		const Vec3f pvec = ray.dir.cross(edge2);

		const float det = edge1.dot(pvec);
		if (fabs(det) < Epsilon) return false;

		const float inv_det = 1.0f / det;

		const Vec3f tvec = ray.org - m_a;
		float lambda = tvec.dot(pvec);
		lambda *= inv_det;

		if (lambda < 0.0f || lambda > 1.0f) return false;

		const Vec3f qvec = tvec.cross(edge1);
		float mue = ray.dir.dot(qvec);
		mue *= inv_det;

		if (mue < 0.0f || mue + lambda > 1.0f) return false;

		float f = edge2.dot(qvec);
		f *= inv_det;
		if (ray.t <= f || f < Epsilon) return false;

		ray.t = f;
		ray.hit = shared_from_this();
		ray.u = lambda;
		ray.v = mue;

		return true;
	}

	virtual void transform(const Mat& t) override {
		// --- PUT YOUR CODE HERE ---
		// Transform vertexes
		//Taken from the OpenRT
		m_a = CTransform::point(m_a, t);
		m_b = CTransform::point(m_b, t);
		m_c = CTransform::point(m_c, t);

		m_na = CTransform::vector(m_na.value(), t);
		m_nb = CTransform::vector(m_nb.value(), t);
		m_nc = CTransform::vector(m_nc.value(), t);

		// Update edges
		m_edge1 = m_b - m_a;
		m_edge2 = m_c - m_a;
	}

	virtual Vec3f getNormal(const Ray& ray) const override
	{
		if (m_na && m_nb && m_nc) {
			return (1.0f - ray.u - ray.v) * m_na.value() + ray.u * m_nb.value() + ray.v * m_nc.value();
		}
		else 
			return normalize(m_edge1.cross(m_edge2));
	}

	virtual Vec2f getTextureCoords(const Ray& ray) const override
	{
		return (1.0f - ray.u - ray.v) * m_ta + ray.u * m_tb + ray.v * m_tc;
	}

	virtual CBoundingBox getBoundingBox(void) const override
	{
		CBoundingBox res;
		res.extend(m_a);
		res.extend(m_b);
		res.extend(m_c);
		return res;
	}


private:
	Vec3f m_a;						///< Position of the first vertex
	Vec3f m_b;						///< Position of the second vertex
	Vec3f m_c;						///< Position of the third vertex
	Vec2f m_ta;						///< Vertex a texture coordiante
	Vec2f m_tb;						///< Vertex b texture coordiante
	Vec2f m_tc;						///< Vertex c texture coordiante
	std::optional<Vec3f> m_na;		///< Normal at vertex a
	std::optional<Vec3f> m_nb;		///< Normal at vertex b
	std::optional<Vec3f> m_nc;		///< Normal at vertex c
	Vec3f m_edge1;					///< Edge AB
	Vec3f m_edge2;					///< Edge AC
};
