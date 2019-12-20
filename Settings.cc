#include "Settings.hh"


Settings Settings::read_settings (istream& is) {
  Settings r;
  string s, v;

  // Version, compared part by part.
  istringstream vs(version());
  while (!vs.eof()) {
    is >> s;
    vs >> v;
    assert(s == v);
  };

  is >> s >> r.nb_players_;
  assert(s == "nb_players");
  assert(r.nb_players_ == 4);

  is >> s >> r.nb_rounds_;
  assert(s == "nb_rounds");
  assert(r.nb_rounds_ >= 1);

  is >> s >> r.nb_dwarves_;
  assert(s == "nb_dwarves");
  assert(r.nb_dwarves_ >= 1);

  is >> s >> r.nb_wizards_;
  assert(s == "nb_wizards");
  assert(r.nb_wizards_ >= 1);

  is >> s >> r.max_nb_orcs_;
  assert(s == "max_nb_orcs");
  assert(r.max_nb_orcs_ >= 1);

  is >> s >> r.nb_trolls_;
  assert(s == "nb_trolls");
  assert(r.nb_trolls_ >= 1);

  is >> s >> r.treasure_value_;
  assert(s == "treasure_value");
  assert(r.treasure_value_ >= 1);

  is >> s >> r.rock_resistance_;
  assert(s == "rock_resistance");
  assert(r.rock_resistance_ >= 1);

  is >> s >> r.rows_;
  assert(s == "rows");
  assert(r.rows_ == 60);

  is >> s >> r.cols_;
  assert(s == "cols");
  assert(r.cols_ == 60);

  return r;
}
