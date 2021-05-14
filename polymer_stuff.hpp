

//namespace polymer
//{
	struct float3
	{
		float x;
		float y;
		float z;

		float3() : x(0), y(0), z(0) {}
		float3(float x, float y, float z) : x(x), y(y), z(z) {}

		float3 operator /(float a) const
		{
			float3 ret(x * a, y * a, z * a);

			return ret;
		}
	};

	inline std::ostream& operator << (std::ostream& o, const float3& v)
	{
		return o << "(" << v.x << "," << v.y << "," << v.z << ")";
	}

	struct float4
	{
		float x;
		float y;
		float z;
		float w;

		float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	};

	inline std::ostream& operator << (std::ostream& o, const float4& v)
	{
		return o << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
	}

	constexpr float dot(const float3& a, const float3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	inline float length(const float3& a) { return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }

	///////////////
	//   plane   //
	///////////////

	static const float PLANE_EPSILON = 0.0001f;

	struct plane
	{
		float4 equation = { 0, 0, 0, 0 }; // ax * by * cz + d form (xyz normal, w distance)
		plane() {}
		plane(const float4& equation) : equation(equation) {}
		plane(const float3& normal, const float& distance) { equation = float4(normal.x, normal.y, normal.z, distance); }
		plane(const float3& normal, const float3& point) { equation = float4(normal.x, normal.y, normal.z, -dot(normal, point)); }
		float3 get_normal() const { return float3(equation.x, equation.y, equation.z); }
		bool is_negative_half_space(const float3& point) const { return (dot(get_normal(), point) < equation.w); }; // +eq.w?
		bool is_positive_half_space(const float3& point) const { return (dot(get_normal(), point) > equation.w); };
		void normalize() { float n = 1.0f / length(get_normal()); equation.x *= n; equation.y *= n; equation.z *= n; equation.w *= n; };
		float get_distance() const { return equation.w; }
		float distance_to(const float3& point) const { return dot(get_normal(), point) + equation.w; };
		bool contains(const float3& point) const { return std::abs(distance_to(point)) < PLANE_EPSILON; };
//		float3 reflect_coord(const float3& c) const { return get_normal() * distance_to(c) * -2.f + c; }
//		float3 reflect_vector(const float3& v) const { return get_normal() * dot(get_normal(), v) * 2.f - v; }
	};

	/////////////
	//   Ray   //
	/////////////

	struct ray
	{
		float3 origin, direction;
		ray() = default;
		ray(const float3& ori, const float3& dir) : origin(ori), direction(dir) {}
		float3 inverse_direction() const { return float3( 1.f / direction.x, 1.f / direction.y, 1.f / direction.z ); }
//		float3 calculate_position(float t) const { return origin + direction * t; }
	};

	inline std::ostream& operator << (std::ostream& o, const ray& r)
	{
		return o << "{" << r.origin << " => " << r.direction << "}";
	}

	inline float3 safe_normalize(const float3& a)
	{
		return a / std::max(0.000001f, length(a));
	}

	inline ray between(const float3& start, const float3& end)
	{
		return{ start, safe_normalize(float3(end.x - start.x, end.y - start.y, end.z - start.z)) };
	}

	//////////////////////////////
	// Ray-object intersections //
	//////////////////////////////

	inline bool intersect_ray_plane(const ray& ray, const plane& p, float3* intersection = nullptr, float* outT = nullptr)
	{
		const float d = dot(ray.direction, p.get_normal());

		// Make sure we're not parallel to the plane
		if (std::abs(d) > PLANE_EPSILON)
		{
			const float t = -p.distance_to(ray.origin) / d;

			if (t >= PLANE_EPSILON)
			{
				if (outT) *outT = t;
				if (intersection) *intersection = float3(ray.origin.x + t * ray.direction.x, ray.origin.y + t * ray.direction.y, ray.origin.z + t * ray.direction.z);
				return true;
			}
		}
		if (outT) *outT = std::numeric_limits<float>::max();
		return false;
	}
//}

