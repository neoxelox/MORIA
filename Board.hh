#ifndef Board_hh
#define Board_hh


#include "Info.hh"
#include "Action.hh"
#include "Random.hh"


/*! \file
 * Contains the Board class, with all the game information,
 * plus the names of the players and a random generator.
 */


/**
 * Manages a random generator and the information of the board.
 */
class Board : public Info, public Random_generator {

  friend class Game;
  friend class SecGame;

  vector<string> names_;
  string generator_;
  vector<bool> killed_;
  vector<vector<char>> M_; // To generate maps.
  vector<Movement> actions_done_;

  void capture (int id, int pl);
  void step (int id, Pos p2);
  static int abso(int x);
  int distance_balrog (Pos p);
  bool adjacent_balrog (Pos p);

  /**
   * Tries to apply a move. Returns true if it could. Marks killed units.
   */
  bool move (int id, Dir dir);

  /**
   * Computes the current number of cells owned by each clan.
   */
  void compute_nb_cells ();

  /**
   * Used by generate_units.
   */
  void new_unit (int id, int pl, Pos pos, UnitType t);
  void ban (Pos pos, int dist, set<Pos>& banned);

  /**
   * Generate all the units of the board.
   */
  void generate_units ();

  /**
   * Used by spawn_units.
   */
  void place (int id, Pos p);

  /**
   * Spaws orcs.
   */
  void spawn_orcs (vector<int> dead_orcs);

  /**
   * Spaws dead dwarves, wizards and trolls.
   */
  void spawn_units (const vector<int>& dead_others);

  /**
   * Moves the orcs.
   */
  void move_orcs ();

  /**
   * Moves the trolls.
   */
  void move_trolls ();

  /**
   * Moves the Balrog.
   */
  void move_balrog ();

  /**
   * Heals units adjacent to wizards.
   */
  void heal_units ();

  /**
   * Reads the generator method, and generates or reads the grid.
   */
  void read_generator_and_grid (istream& is);

  /**
   * Generates a board at random.
   */
  void generator (vector<int> param);

  /**
   * Prints some information of the unit.
   */
  inline static void print_unit (Unit u, ostream& os) {
    os << ut2char(u.type) << ' '
       << u.player << ' '
       << u.pos.i << ' '
       << u.pos.j << ' '
       << u.health << endl;
  }

  /**
   * Used to generate random maps.
   */
  void try_cell (int i, int j, int r, int c, queue<Pos>& seg, set<Pos>& S);
  bool tunnel ();
  bool path (int x, int y);
  bool generate_paths ();
  void max_area (const vector<int>& V, int mind, int mina,
                 int& j1, int& j2, int& h);
  bool find_space (int par1, int par2, int par3,
                   int& x1, int& x2, int& y1, int& y2, set<Pos>& free);
  bool maze (set<Pos>& free, int& nb_tre);
  bool chess (set<Pos>& free, int& nb_tre);
  void generate_grid ();


public:

  /**
   * Returns the name of a player.
   */
  inline string name (int player) const {
    assert(player_ok(player));
    return names_[player];
  }

  /**
   * Construct a board by reading information from a stream.
   */
  Board (istream& is, int seed);

  /**
   * Prints the board preamble to a stream.
   */
  void print_preamble (ostream& os) const;

  /**
   * Prints the name players to a stream.
   */
  void print_names (ostream& os) const;

  /**
   * Prints the state of the board to a stream.
   */
  void print_state (ostream& os) const;

  /**
   * Prints the results and the names of the winning players.
   */
  void print_results () const;

  /**
   * Computes the next board aplying the given actions to the current board.
   * It also prints to os the actual actions performed.
   */
  void next (const vector<Action>& act, ostream& os);

};


#endif
