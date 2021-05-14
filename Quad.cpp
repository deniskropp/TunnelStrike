#include "Quad.hpp"


namespace TunnelStrike {


	Quad::Quad(sf::Vector3f a, sf::Vector3f b, sf::Vector3f c, sf::Vector3f d, sf::Color ca, sf::Color cb, sf::Color cc, sf::Color cd)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
		v[3] = d;

		color[0] = ca;
		color[1] = cb;
		color[2] = cc;
		color[3] = cd;

		segments.push_back(getSegment(0, 1));
		segments.push_back(getSegment(1, 2));
		segments.push_back(getSegment(2, 3));
		segments.push_back(getSegment(3, 0));
	}

	Segment3d Quad::getSegment(int v1, int v2) const
	{
		return Segment3d(Vector3d(v[v1].x, v[v1].y, v[v1].z), Vector3d(v[v2].x, v[v2].y, v[v2].z), color[v1], color[v2]);
	}


}