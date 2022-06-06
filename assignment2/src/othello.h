#pragma once
#include <array>
#include <string>
#include <vector>

#include "renderer.h"

namespace Othello {

	enum class PlayerType { HUMAN, MINIMAX, NONE };
	PlayerType GetPlayerType(const std::string&);

	enum class Piece { NONE, LIGHT, DARK };
	enum class Directions { N, NE, E, SE, S, SW, W, NW };
	constexpr std::array<Directions, 8> Dirs{Directions::N, Directions::NE, Directions::E, Directions::SE, Directions::S, Directions::SW, Directions::W, Directions::NW};
	struct Board {
		Piece pieces[4][4] = {
			{Piece::NONE, Piece::NONE, Piece::NONE, Piece::NONE},
			{Piece::NONE, Piece::DARK, Piece::LIGHT, Piece::NONE},
			{Piece::NONE, Piece::LIGHT, Piece::DARK, Piece::NONE},
			{Piece::NONE, Piece::NONE, Piece::NONE, Piece::NONE}
		};
		Board() = default;
		Board(const Board&, const glm::ivec2&, Piece);
		bool operator ==(const Board&);
		bool operator !=(const Board&);

		void render();

		std::vector<Board> successors(Piece) const;
		bool isTerminal(const std::vector<Board>&, const Piece) const;
		inline bool inBoard(const glm::ivec2& position) const {
			return position.x >= 0 && position.x < 4 && position.y >= 0 && position.y < 4;
		}
	};

	struct Mouse {
		static int MouseX, MouseY;
		static void MoveCallback(double, double);
		static void ButtonCallback(bool);
	};

	void RenderBoard();
	void RenderShadowPieces();

	bool ObtainPlayers(const char* const * const args);
	void Update();
};