#include "Player.hh"


void Player::reset (ifstream& is) {
  *(Action*)this = Action();

  read_grid(is);

  string s;
  is >> s >> round_;
  assert(s == "round");
  assert(round_ >= 0 and round_ < nb_rounds());

  int np = nb_players();
  is >> s;
  assert(s == "nb_cells");
  nb_cells_ = vector<int>(np, 0);
  for (int& nc : nb_cells_) {
    is >> nc;
    assert(nc >= 0);
  }

  is >> s;
  assert(s == "nb_treasures");
  nb_treasures_ = vector<int>(np, 0);
  for (int& ts : nb_treasures_) {
    is >> ts;
    assert(ts >= 0);
  }

  is >> s;
  assert(s == "status");
  cpu_status_ = vector<double>(np, 0);
  for (double& st : cpu_status_) {
    is >> st;
    assert(st == -1 or (st >= 0 and st <= 1));
  }

  unit_ = vector<Unit>(np*(nb_dwarves() + nb_wizards())
                       + max_nb_orcs() + nb_trolls() + 1);

  for (int id = 0; id < nb_units(); ++id) {
    char type;
    int player, i, j, health;
    _my_assert(is >> type >> player >> i >> j >> health,
               "Could not read info for unit " + int_to_string(id) + ".");

    assert(type == 'd' or type == 'w'
           or type == 'o' or type == 't' or type == 'b');
    UnitType ut = char2ut(type);
    if (ut == Dwarf or ut == Wizard) assert(player >= 0 and player < np);
    else assert(player == -1);

    if (ut == Orc and health == 0) assert(i == -1 and j == -1);
    else {
      CellType ct = grid_[i][j].type;
      bool treasure = grid_[i][j].treasure;
      if (ct != Cave) assert(not treasure);
      assert(pos_ok(i, j) and grid_[i][j].id == -1);

      if (ut == Balrog) assert(ct != Outside);
      else {
        assert(ct != Granite and ct != Rock);
        if (ut == Orc) assert(ct != Outside);
        else assert(ct != Abyss);
        assert(health > 0);
      }
      if (ut == Dwarf) assert(not treasure and health <= dwarves_health());
      else if (ut == Wizard) assert(health <= wizards_health());
      else if (ut == Orc) assert(health <= orcs_health());
      else if (ut == Troll) assert(health <= trolls_health());

      grid_[i][j].id = id;
    }

    unit_[id] = Unit(ut, id, player, health, Pos(i, j));
  }

  update_vectors_by_player();
}
