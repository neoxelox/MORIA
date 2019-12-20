#ifndef Settings_hh
#define Settings_hh


#include "Structs.hh"


/** \file
 * Contains a class to store all the game settings that do not change
 * during a game, except the names of the players.
 */


/**
 * Stores most of the game settings.
 */
class Settings {

  friend class Info;
  friend class Board;
  friend class Game;
  friend class SecGame;
  friend class Player;

  static const int dwarves_health_     = 100;
  static const int wizards_health_     = 50;
  static const int orcs_health_        = 75;
  static const int trolls_health_      = 500;
  static const int min_damage_dwarves_ = 20;
  static const int max_damage_dwarves_ = 40;
  static const int min_damage_orcs_    = 15;
  static const int max_damage_orcs_    = 30;
  static const int min_damage_trolls_  = 50;
  static const int max_damage_trolls_  = 150;
  static const int ini_treasures_      = 80;
  static const int inv_prob_abyss_     = 25;
  static const int inv_prob_orc_       = 50;
  int nb_players_;
  int nb_rounds_;
  int nb_dwarves_;
  int nb_wizards_;
  int max_nb_orcs_;
  int nb_trolls_;
  int treasure_value_;
  int rock_resistance_;
  int rows_;
  int cols_;

  /**
   * Reads the settings from a stream.
   */
  static Settings read_settings (istream& is);

public:

  /**
   * Returns a string with the game name and version.
   */
  inline static string version () {
    return "Moria 1.1";
  }

  /**
   * Returns the number of players in the game.
   */
  inline int nb_players () const {
    return nb_players_;
  }

  /**
   * Returns the total number of rounds of a game.
   */
  inline int nb_rounds () const {
    return nb_rounds_;
  }

  /**
   * Returns the initial number of dwarves for every player.
   */
  inline int nb_dwarves () const {
    return nb_dwarves_;
  }

  /**
   * Returns the initial number of wizards for every player.
   */
  inline int nb_wizards () const {
    return nb_wizards_;
  }

  /**
   * Returns the maximum number of orcs.
   */
  inline int max_nb_orcs () const {
    return max_nb_orcs_;
  }

  /**
   * Returns the constant number of trolls.
   */
  inline int nb_trolls () const {
    return nb_trolls_;
  }

  /**
   * Returns the value of a treasure w.r.t. the value of a conquered cell.
   */
  inline int treasure_value () const {
    return treasure_value_;
  }

  /**
   * Returns the number of 'attacks' needed to excavate a rock.
   */
  inline int rock_resistance () const {
    return rock_resistance_;
  }

  /**
   * Returns the number of rows of the grid of the game.
   */
  inline int rows () const {
    return rows_;
  }

  /**
   * Returns the number of columns of the grid of the game.
   */
  inline int cols () const {
    return cols_;
  }

  /**
   * Returns the initial (and maximum) health of the dwarves.
   */
  inline int dwarves_health () const {
    return dwarves_health_;
  }

  /**
   * Returns the initial (and maximum) health of the wizards.
   */
  inline int wizards_health () const {
    return wizards_health_;
  }

  /**
   * Returns the initial (and maximum) health of the orcs.
   */
  inline int orcs_health () const {
    return orcs_health_;
  }

  /**
   * Returns the initial (and maximum) health of the trolls.
   */
  inline int trolls_health () const {
    return trolls_health_;
  }

  /**
   * Returns the minimum damage inflicted by a dwarf attack.
   */
  inline int min_damage_dwarves () const {
    return min_damage_dwarves_;
  }

  /**
   * Returns the maximum damage inflicted by a dwarf attack.
   */
  inline int max_damage_dwarves () const {
    return max_damage_dwarves_;
  }

  /**
   * Returns the minimum damage inflicted by an orc attack.
   */
  inline int min_damage_orcs () const {
    return min_damage_orcs_;
  }

  /**
   * Returns the maximum damage inflicted by an orc attack.
   */
  inline int max_damage_orcs () const {
    return max_damage_orcs_;
  }

  /**
   * Returns the minimum damage inflicted by a troll attack.
   */
  inline int min_damage_trolls () const {
    return min_damage_trolls_;
  }

  /**
   * Returns the maximum damage inflicted by a troll attack.
   */
  inline int max_damage_trolls () const {
    return max_damage_trolls_;
  }

  /**
   * Returns the initial number of treasures on the board.
   */
  inline int ini_treasures () const {
    return ini_treasures_;
  }

  /**
   * Returns 1/(the probability of a rock becoming an abyss).
   */
  inline int inv_prob_abyss () const {
    return inv_prob_abyss_;
  }

  /**
   * Returns 1/(the probability of an abyss producing an orc at each round).
   */
  inline int inv_prob_orc () const {
    return inv_prob_orc_;
  }

  /**
   * Returns whether pl is a valid player identifier.
   */
  inline bool player_ok (int pl) const {
    return pl >= 0 and pl < nb_players();
  }

  /**
   * Returns whether (i, j) is a position inside the board.
   */
  inline bool pos_ok (int i, int j) const {
    return i >= 0 and i < rows() and j >= 0 and j < cols();
  }

  /**
   * Returns whether p is a position inside the board.
   */
  inline bool pos_ok (Pos p) const {
    return pos_ok(p.i, p.j);
  }

};


#endif
