#ifndef Info_hh
#define Info_hh


#include "Settings.hh"
#include "State.hh"


/*! \file
 * Contains a class to store most of the information of the game.
 */


/**
 * Stores all the information of the game,
 * except the names of the vectors and the random generator of the board.
 */
class Info : public Settings, public State {

  friend class Game;
  friend class SecGame;

public:

  /**
   * Returns the cell defined by the char c.
   */
  inline Cell char2cell (char c) const {
    Cell cell;
    if (c == 'R') {
      cell.type = Rock;
      cell.turns = rock_resistance();
    }
    else if (c == 'C' or c == 'T') {
      cell.type = Cave;
      if (c == 'T') cell.treasure = true;
    }
    else if (c == 'O') cell.type = Outside;
    else if (c == 'G') cell.type = Granite;
    else if (c == 'A') cell.type = Abyss;
    else if (c >= '0' and c <= '3') {
      cell.type = Cave;
      cell.owner = c - '0';
    }
    else if (c >= 'a' and c <= 'a' + rock_resistance() - 2) {
      cell.type = Rock;
      cell.turns = c - 'a' + 1;
    }
    else _my_assert(false, string(1, c) + " in grid definition.");
    return cell;
  }

  /**
   * Reads the grid of the board.
   */
  void read_grid (istream& is) {
    int r = rows();
    int c = cols();
    grid_ = vector<vector<Cell>>(r, vector<Cell>(c));
    for (int i = 0; i < r; ++i) {
      string s;
      is >> s;
      _my_assert((int)s.size() == c,
                 "The read map has a line with incorrect length.");
      for (int j = 0; j < c; ++j) grid_[i][j] = char2cell(s[j]);
    }
  }

  /**
   * Called to update the auxiliar redundant vectors.
   */
  void update_vectors_by_player () {
    int np = nb_cells_.size();
    dwarves_ = wizards_ = vector<vector<int>>(np);
    orcs_ = trolls_ = vector<int>(0);
    for (const Unit& u : unit_) {
      UnitType ut = u.type;
      _my_assert(ut != UnitTypeSize, "Wrong unit type on vectors update.");
      int up = u.player;
      if (ut == Dwarf or ut == Wizard) _my_assert(up >= 0 and up < np, "Clan unit of wrong player.");
      if (ut != Dwarf and ut != Wizard) _my_assert(up == -1, "Sauron unit of player != -1.");
      if (ut == Dwarf) dwarves_[up].push_back(u.id);
      else if (ut == Wizard) wizards_[up].push_back(u.id);
      else if (ut == Troll) trolls_.push_back(u.id);
      else if (ut == Balrog) balrog_id_ = u.id;
      else if (u.health > 0) orcs_.push_back(u.id);
    }
  }

};


#endif
