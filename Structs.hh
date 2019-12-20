#ifndef Structs_hh
#define Structs_hh


#include "Utils.hh"


/** \file
 * Contains the Dir enumeration, the Pos struct,
 * the CellType enumeration, the Cell struct,
 * the UnitType enumeration, the Unit struct,
 * and some useful little functions.
 */


/**
 * Enum to encode directions.
 */
enum Dir {
  Bottom, BR, Right, RT, Top, TL, Left, LB,
  None,
  DirSize
};


/**
 * Returns whether dir is a valid direction.
 */
inline bool dir_ok (Dir dir) {
  return dir >= Bottom and dir <= None;
}


/**
 * Simple struct to handle positions.
 */
struct Pos {

  int i, j;

  /**
   * Default constructor (0, 0).
   */
  inline Pos () : i(0), j(0) { }

  /**
   * Given constructor.
   */
  inline Pos (int i, int j) : i(i), j(j) { }

  /**
   * Print operator.
   */
  inline friend ostream& operator<< (ostream& os, const Pos& p) {
    return os << "(" << p.i << ", " << p.j << ")";
  }

  /**
   * Comparison operator.
   */
  inline friend bool operator== (const Pos& a, const Pos& b) {
    return a.i == b.i and a.j == b.j;
  }

  /**
   * Comparison operator.
   */
  inline friend bool operator!= (const Pos& a, const Pos& b) {
    return not (a == b);
  }

  /**
   * Comparison operator, mostly needed for sorting.
   */
  inline friend bool operator< (const Pos& a, const Pos& b) {
    if (a.i != b.i) return a.i < b.i;
    return a.j < b.j;
  }

  /**
   * Increment operator: moves a position according to a direction.
   */
  inline Pos& operator+= (Dir d) {
    switch (d) {
      case Bottom: ++i;  break;
      case BR: ++i; ++j; break;
      case Right:  ++j;  break;
      case RT: --i; ++j; break;
      case Top:    --i;  break;
      case TL: --i; --j; break;
      case Left:   --j;  break;
      case LB: ++i; --j; break;
      case None:         break;
      default: ; // do nothing
    }
    return *this;
  }

  /**
   * Addition operator: returns a position by adding a direction.
   */
  inline Pos operator+ (Dir d) const {
    Pos p = *this;
    p += d;
    return p;
  }

  /**
   * Increment operator: moves a position according to another position.
   */
  inline Pos& operator+= (Pos p) {
    this->i += p.i;
    this->j += p.j;
    return *this;
  }

  /**
   * Addition operator: returns a position by adding another position.
   */
  inline Pos operator+ (Pos p) const {
    Pos p2 = *this;
    p2 += p;
    return p2;
  }

};


/**
 * Defines the type of a cell.
 */
enum CellType {
  Outside, Cave, Abyss, Granite, Rock,
  CellTypeSize
};


/**
 * Describes a cell in the board, and its contents.
 */
struct Cell {

  CellType type; // The kind of cell.
  int owner;     // The player that owns it, otherwise -1.
  int id;        // The id of a unit if present, otherwise -1.
  int turns;     // For a rock, times it has to be attacked to destroy it.
  bool treasure; // For a cave, if it has a treasure or not.

  /**
   * Default constructor (Cave, -1, -1, 0).
   */
  inline Cell () : type(Cave), owner(-1), id(-1), turns(0), treasure(false) { }

  /**
   * Given constructor.
   */
  inline Cell (CellType t, int ow, int id, int tu, bool tr)
               : type(t), owner(ow), id(id), turns(tu), treasure(tr) { }

};


/**
 * Defines the type of a unit.
 */
enum UnitType {
  Dwarf, Wizard, Orc, Troll, Balrog,
  UnitTypeSize
};


/**
 * Returns the char corresponding to a unit type.
 */
inline char ut2char (UnitType u) {
  if (u == Dwarf) return 'd';
  if (u == Wizard) return 'w';
  if (u == Orc) return 'o';
  if (u == Troll) return 't';
  if (u == Balrog) return 'b';
  _unreachable();
}


/**
 * Returns the unit type corresponding to a char.
 */
inline UnitType char2ut (char c) {
  if (c == 'd') return Dwarf;
  if (c == 'w') return Wizard;
  if (c == 'o') return Orc;
  if (c == 't') return Troll;
  if (c == 'b') return Balrog;
  _unreachable();
}


/**
 * Describes a unit on the board and its properties.
 */
struct Unit {

  UnitType type; // The kind of unit.
  int id;        // The id for this unit (new orcs may repeat old ids).
  int player;    // The player that owns this unit.
  int health;    // For the Balrog, anything. For the rest, the current health.
  Pos pos;       // The position inside the board.

  /**
   * Default constructor (Dwarf, -1, -1, 0, (0, 0)).
   */
  inline Unit () : type(Dwarf), id(-1), player(-1), health(0), pos(-1, -1) { }

  /**
   * Given constructor.
   */
  inline Unit (UnitType t, int id, int pl, int h, Pos p)
               : type(t), id(id), player(pl), health(h), pos(p) { }

};


#endif
