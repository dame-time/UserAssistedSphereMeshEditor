//
// Created by Davide Paollilo on 08/03/24.
//

#pragma once

#include <functional>

namespace Renderer {
	struct Triangle
	{
		int i, j, k;
		
		Triangle() : i(0), j(0), k(0) {}
		Triangle(int _i, int _j, int _k) : i(_i), j(_j), k(_k) {}
		
		bool operator==(const Triangle& other) const {
			std::array<int, 3> vertices1{{i, j, k}};
			std::array<int, 3> vertices2{{other.i, other.j, other.k}};
			std::sort(vertices1.begin(), vertices1.end());
			std::sort(vertices2.begin(), vertices2.end());
			return vertices1 == vertices2;
		}
	};
	
	struct Edge
	{
		int i, j;
		
		Edge() : i(0), j(0) {}
		Edge(int first, int second)
		{
			i = first;
			j = second;
		}
		
		bool operator==(const Edge &other) const {
			return (i == other.i && j == other.j) || (i == other.j && j == other.i);
		}
	};
};

namespace std {
	template<>
	struct hash<Renderer::Edge> {
		std::size_t operator()(const Renderer::Edge& edge) const {
			int ordered_i = std::min(edge.i, edge.j);
			int ordered_j = std::max(edge.i, edge.j);
			
			std::size_t h1 = std::hash<int>{}(ordered_i);
			std::size_t h2 = std::hash<int>{}(ordered_j);
			
			return h1 ^ (h2 << 1);
		}
	};
	
	template<>
	struct hash<Renderer::Triangle> {
		std::size_t operator()(const Renderer::Triangle& triangle) const {
			std::array<int, 3> vertices{{triangle.i, triangle.j, triangle.k}};
			std::sort(vertices.begin(), vertices.end());
			
			std::size_t h1 = std::hash<int>{}(vertices[0]);
			std::size_t h2 = std::hash<int>{}(vertices[1]);
			std::size_t h3 = std::hash<int>{}(vertices[2]);
			
			return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
		}
	};
}
