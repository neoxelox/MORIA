#include "Player.hh"

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME AIBOOMER

struct PLAYER_NAME : public Player
{

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player *factory() {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  /**
   * Constants and Definitions
   */

#define DEBUG false

#define COLS 60
#define ROWS 60

#define NONE_WEIGHT 999999

#define OUTSIDE_WEIGHT_DWARF 3
#define CAVE_NOWNED_WEIGHT_DWARF 2
#define CAVE_OWNED_WEIGHT_DWARF 3
#define TREASURE_WEIGHT_DWARF 1
#define ROCK_WEIGHT_DWARF 7

#define OUTSIDE_WEIGHT_WIZARD 1
#define CAVE_NOWNED_WEIGHT_WIZARD 1
#define CAVE_OWNED_WEIGHT_WIZARD 1
#define TREASURE_WEIGHT_WIZARD 1
#define ROCK_WEIGHT_WIZARD 999

#define GRANITE_WEIGHT 999
#define ABYSS_WEIGHT 999

#define UNIT_OWNED_WEIGHT 999

#define DWARF_NOWNED_WEIGHT_DWARF 5
#define WIZARD_NOWNED_WEIGHT_DWARF 4
#define ORC_WEIGHT_DWARF 6

#define DWARF_NOWNED_WEIGHT_WIZARD 999
#define WIZARD_NOWNED_WEIGHT_WIZARD 99
#define ORC_WEIGHT_WIZARD 999

#define TROLL_WEIGHT 99
#define BALROG_WEIGHT 999

#define MIN_NUM_CLAN_DWARVES 100
#define MAX_ROUND_ATTACK 200
#define MAX_TREASURES_LEFT 5
#define MAX_ENEMY_CELLS 50

  typedef vector<int>  VI;
  typedef vector<Pos>  VP;
  typedef vector<Dir>  VD;
  typedef vector<Cell> VC;
  typedef vector<Unit> VU;
  typedef vector<bool> VB;
  typedef vector<VB>   MB;

  struct WeightPos {
    Pos p;
    int w;
    Pos b;

    inline friend bool operator< (const WeightPos &a, const WeightPos &b) {
      return (a.w > b.w);
    }
  };

  typedef vector<WeightPos> VW;
  typedef vector<VW> MW;

  enum Priorities {
    NownedCaves, Treasures, Enemies, Nop,
    PrioritiesSize
  };

  typedef vector<Priorities> VPI;

  enum Status {
    Idle, Pathing, Mining, Fighting, Healing,
    StatusSize
  };

  struct Sunit {
    Unit u;
    Status s;
    VPI p;
    VP path;
  };

  typedef vector<Sunit> VS;

  /**
   * Attributes
   */

  int player1ID;
  int player2ID;
  int player3ID;
  int sauronID;

  int treasuresLeft;
  int maxEnemyCells;
  int maxEnemy;

  map<int, VP> DwarvesPaths;
  map<int, VP> WizardsPaths;

  VU Dwarves;
  VD wizardDirs;

  MW WeightMap;
  MW WeightMapWizard;
  MW WeightGraph;
  MB VisitedPos;

  /**
   * Debug Output
   */

  void printd(string msg) {
    if (DEBUG)
      cerr << msg << endl;
  }

  string logMSG() {
    string gen_info = "===[ROUND " + to_string(round()) + "]===";
    string mep = "[ME] : [CELLS " + to_string(nb_cells(me())) + "] | " + "[DWARVES " + to_string(dwarves(me()).size()) + "] | " + "[WIZARDS " + to_string(wizards(me()).size()) + "] | " + "[TREASURES " + to_string(nb_treasures(me())) + "]";
    string player1 = "[PLAYER 1] : [CELLS " + to_string(nb_cells(player1ID)) + "] | " + "[DWARVES " + to_string(dwarves(player1ID).size()) + "] | " + "[WIZARDS " + to_string(wizards(player1ID).size()) + "] | " + "[TREASURES " + to_string(nb_treasures(player1ID)) + "]";
    string player2 = "[PLAYER 2] : [CELLS " + to_string(nb_cells(player2ID)) + "] | " + "[DWARVES " + to_string(dwarves(player2ID).size()) + "] | " + "[WIZARDS " + to_string(wizards(player2ID).size()) + "] | " + "[TREASURES " + to_string(nb_treasures(player2ID)) + "]";
    string player3 = "[PLAYER 3] : [CELLS " + to_string(nb_cells(player3ID)) + "] | " + "[DWARVES " + to_string(dwarves(player3ID).size()) + "] | " + "[WIZARDS " + to_string(wizards(player3ID).size()) + "] | " + "[TREASURES " + to_string(nb_treasures(player3ID)) + "]";
    string sauron = "[SAURON] : [ORCS " + to_string(orcs().size()) + "] | " + "[TROLLS " + to_string(trolls().size()) + "]";

    return gen_info + "\n" + mep + "\n" + player1 + "\n" + player2 + "\n" + player3 + "\n" + sauron;
  }

  void printVI(VI &V) {
    string tmp;
    printd("---Generic Vector---");
    for (int e : V)
    {
      tmp += to_string(e) + " ";
    }
    printd(tmp);
    printd("-------------------");
  }

  void printVP(VP &V) {
    string tmp;
    printd("---Position Vector---");
    for (Pos e : V)
    {
      tmp += to_string(e.i) + "," + to_string(e.j) + " ";
    }
    printd(tmp);
    printd("---------------------");
  }

  void printVD(VD &V) {
    string tmp;
    printd("---Direction Vector---");
    for (Dir e : V)
    {
      switch (e)
      {
      case Bottom:
        tmp += "B ";
        break;
      case BR:
        tmp += "BR ";
        break;
      case Right:
        tmp += "R ";
        break;
      case RT:
        tmp += "RT ";
        break;
      case Top:
        tmp += "T ";
        break;
      case TL:
        tmp += "TL ";
        break;
      case Left:
        tmp += "L ";
        break;
      case LB:
        tmp += "LB ";
        break;
      default:
        tmp += "None ";
        break;
      }
    }
    printd(tmp);
    printd("----------------------");
  }

  void printVC(VC &V) {
    string tmp;
    printd("---Cell Vector---");
    for (Cell e : V)
    {
      tmp += "< ";
      switch (e.type)
      {
      case Outside:
        tmp += "O ";
        break;
      case Cave:
        tmp += "C ";
        break;
      case Abyss:
        tmp += "A ";
        break;
      case Granite:
        tmp += "G ";
        break;
      case Rock:
        tmp += "R ";
        break;
      default:
        tmp += "None ";
        break;
      }

      if (e.owner == me())
        tmp += "ME ";
      else if (e.owner == player1ID)
        tmp += "P1 ";
      else if (e.owner == player2ID)
        tmp += "P2 ";
      else if (e.owner == player3ID)
        tmp += "P3 ";
      else if (e.owner == -1)
        tmp += "SA ";
      else
        tmp += "None ";

      Unit u = unit(e.id);

      switch (u.type)
      {
      case Dwarf:
        tmp += "D ";
        break;
      case Wizard:
        tmp += "W ";
        break;
      case Orc:
        tmp += "O ";
        break;
      case Troll:
        tmp += "T ";
        break;
      case Balrog:
        tmp += "B ";
        break;
      default:
        tmp += "None ";
        break;
      }

      if (e.type == Rock)
        tmp += to_string(e.turns) + " ";
      if (e.type == Cave)
        tmp += to_string(e.treasure) + " ";

      tmp += "> ";
    }
    printd(tmp);
    printd("----------------");
  }

  void printVU(VU &V) {
    string tmp;
    printd("---Cell Vector---");
    for (Unit e : V)
    {
      tmp += "< ";
      switch (e.type)
      {
      case Dwarf:
        tmp += "D ";
        break;
      case Wizard:
        tmp += "W ";
        break;
      case Orc:
        tmp += "O ";
        break;
      case Troll:
        tmp += "T ";
        break;
      case Balrog:
        tmp += "B ";
        break;
      default:
        tmp += "None ";
        break;
      }

      tmp += to_string(e.id) + " ";

      if (e.player == me())
        tmp += "ME ";
      else if (e.player == player1ID)
        tmp += "P1 ";
      else if (e.player == player2ID)
        tmp += "P2 ";
      else if (e.player == player3ID)
        tmp += "P3 ";
      else if (e.player == -1)
        tmp += "SA ";
      else
        tmp += "None ";

      tmp += to_string(e.health) + " ";

      tmp += to_string(e.pos.i) + "-" + to_string(e.pos.j) + " ";

      tmp += "> ";
    }
    printd(tmp);
    printd("----------------");
  }

  /**
   * Utility functions
   */

  string dirTOstr(Dir d) {
    switch (d)
    {
    case Bottom:
      return "Bottom";
      break;
    case BR:
      return "Bottom Right";
      break;
    case Right:
      return "Right";
      break;
    case RT:
      return "Right Top";
      break;
    case Top:
      return "Top";
      break;
    case TL:
      return "Top Left";
      break;
    case Left:
      return "Left";
      break;
    case LB:
      return "Left Bottom";
      break;
    default:
      return "None";
      break;
    }
    return "None";
  }

  VU viTOvu(VI &V) {
    VU units;
    for (int id : V)
      units.push_back(unit(id));
    return units;
  }

  VC vpTOvc(VP &V) {
    VC cells;
    for (Pos e : V)
      cells.push_back(cell(e));
    return cells;
  }

  VP vdTOvp(VD &V, Pos current_pos) {
    int vsize = V.size();
    VP poss(vsize);
    for (int i = 0; i < vsize; i++)
    {
      current_pos += V[i];
      poss[i] = current_pos;
    }
    return poss;
  }

  Dir randDir(VD &V) {
    return V[random(0,V.size()-1)];
  }

  /**
   * Logic functions
   */

  Dir posDir(Pos current_pos, Pos go_pos) {
    int i = current_pos.i - go_pos.i;
    int j = current_pos.j - go_pos.j;
    if (i > 0 && j > 0)
      return TL;
    else if (i > 0 && j == 0)
      return Top;
    else if (i > 0 && j < 0)
      return RT;
    else if (i == 0 && j > 0)
      return Left;
    else if (i == 0 && j < 0)
      return Right;
    else if (i < 0 && j > 0)
      return LB;
    else if (i < 0 && j == 0)
      return Bottom;
    else if (i < 0 && j < 0)
      return BR;
    else
      return None;
  }

  Pos findNearestPos(VP &V, Pos current_pos) {
    if (V.size() < 1)
      return current_pos;
    Pos nearestPos = V[0];
    for (Pos dst_pos : V)
    {
      if (abs(dst_pos.i - current_pos.i) + abs(dst_pos.j - current_pos.j) < abs(nearestPos.i - current_pos.i) + abs(nearestPos.j - current_pos.j))
      {
        nearestPos = dst_pos;
      }
    }

    return nearestPos;
  }

  void clearMB(MB &M) {
    int mrsize = M.size();
    int mcsize = M[0].size();
    for (int i = 0; i < mrsize; i++) {
      for (int j = 0; j < mcsize; j++) {
        M[i][j] = false;
      }
    }
  }

  Pos findEnemy(Pos current_pos, int player, int health) {
    clearMB(VisitedPos);
    Pos backEnemyPos; backEnemyPos.i = -1; backEnemyPos.j = -1;
    queue<Pos> posq;
    posq.push(current_pos+TL);posq.push(current_pos+Top);posq.push(current_pos+RT);posq.push(current_pos+Left);posq.push(current_pos+Right);posq.push(current_pos+LB);posq.push(current_pos+Bottom);posq.push(current_pos+BR);
    VisitedPos[current_pos.i][current_pos.j] = true;

    while(posq.size() > 0) {
      if(!pos_ok(posq.front()) || VisitedPos[posq.front().i][posq.front().j]) {
        posq.pop();
        continue;
      }

      Cell current_cell = cell(posq.front());
      if(current_cell.type == Abyss || current_cell.type == Granite) {
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_cell.id == -1) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      Unit current_unit = unit(current_cell.id);
      if(current_unit.player == -1 || current_unit.player == me()) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_unit.player != player && player != -1) {
        if(backEnemyPos.i == -1) backEnemyPos = posq.front();
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_unit.health > health && health != -1) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      return posq.front();
    }

    return backEnemyPos;
  }

  Pos findAlly(Pos current_pos, int health) {
    clearMB(VisitedPos);
    Pos backAllyPos; backAllyPos.i = -1; backAllyPos.j = -1;
    queue<Pos> posq;
    posq.push(current_pos+TL);posq.push(current_pos+Top);posq.push(current_pos+RT);posq.push(current_pos+Left);posq.push(current_pos+Right);posq.push(current_pos+LB);posq.push(current_pos+Bottom);posq.push(current_pos+BR);
    VisitedPos[current_pos.i][current_pos.j] = true;

    while(posq.size() > 0) {
      if(!pos_ok(posq.front()) || VisitedPos[posq.front().i][posq.front().j]) {
        posq.pop();
        continue;
      }

      Cell current_cell = cell(posq.front());
      if(current_cell.type == Abyss || current_cell.type == Granite || current_cell.type == Rock) {
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_cell.id == -1) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      Unit current_unit = unit(current_cell.id);
      if(current_unit.player != me() or current_unit.type == Wizard) {
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_unit.health > health && health != -1) {
        if(backAllyPos.i == -1) backAllyPos = posq.front();
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      return posq.front();
    }

    return backAllyPos;
  }

  Pos findNownedCaves(Pos current_pos, int player, bool isEmpty) {
    clearMB(VisitedPos);
    Pos backNownedCavePos; backNownedCavePos.i = -1; backNownedCavePos.j = -1;
    queue<Pos> posq;
    posq.push(current_pos+TL);posq.push(current_pos+Top);posq.push(current_pos+RT);posq.push(current_pos+Left);posq.push(current_pos+Right);posq.push(current_pos+LB);posq.push(current_pos+Bottom);posq.push(current_pos+BR);
    VisitedPos[current_pos.i][current_pos.j] = true;

    while(posq.size() > 0) {
      if(!pos_ok(posq.front()) || VisitedPos[posq.front().i][posq.front().j]) {
        posq.pop();
        continue;
      }

      Cell current_cell = cell(posq.front());
      if(current_cell.treasure) return posq.front();
      
      if(current_cell.type == Abyss || current_cell.type == Granite) {
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      } else if(current_cell.type != Cave) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_cell.id != -1 && isEmpty) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if (current_cell.id != -1) {
        Unit current_unit = unit(current_cell.id);
        if (current_unit.player == -1 || current_unit.player == me()) {
          VisitedPos[posq.front().i][posq.front().j] = true;
          posq.pop();
          continue;
        }
      }

      if (current_cell.owner == me()) {
          posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
          VisitedPos[posq.front().i][posq.front().j] = true;
          posq.pop();
          continue;
      }

      if (current_cell.owner != player && player != -1) {
          if (backNownedCavePos.i == -1) backNownedCavePos = posq.front();
          posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
          VisitedPos[posq.front().i][posq.front().j] = true;
          posq.pop();
          continue;
      }

      return posq.front();
    }

    return backNownedCavePos;
  }

  Pos findTreasures(Pos current_pos, int player, bool isEmpty) {
    clearMB(VisitedPos);
    Pos backNownedCavePos; backNownedCavePos.i = -1; backNownedCavePos.j = -1;
    queue<Pos> posq;
    posq.push(current_pos+TL);posq.push(current_pos+Top);posq.push(current_pos+RT);posq.push(current_pos+Left);posq.push(current_pos+Right);posq.push(current_pos+LB);posq.push(current_pos+Bottom);posq.push(current_pos+BR);
    VisitedPos[current_pos.i][current_pos.j] = true;

    while(posq.size() > 0) {
      if(!pos_ok(posq.front()) || VisitedPos[posq.front().i][posq.front().j]) {
        posq.pop();
        continue;
      }

      Cell current_cell = cell(posq.front());
      if(current_cell.treasure) return posq.front();
      
      if(current_cell.type == Abyss || current_cell.type == Granite) {
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      } else if(current_cell.type != Cave) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if(current_cell.id != -1 && isEmpty) {
        posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
        VisitedPos[posq.front().i][posq.front().j] = true;
        posq.pop();
        continue;
      }

      if (current_cell.id != -1) {
        Unit current_unit = unit(current_cell.id);
        if (current_unit.player == -1 || current_unit.player == me()) {
          VisitedPos[posq.front().i][posq.front().j] = true;
          posq.pop();
          continue;
        }
      }

      if (current_cell.owner == me()) {
          posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
          VisitedPos[posq.front().i][posq.front().j] = true;
          posq.pop();
          continue;
      }

      if (current_cell.owner != player && player != -1) {
          if (backNownedCavePos.i == -1) backNownedCavePos = posq.front();
          posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
          VisitedPos[posq.front().i][posq.front().j] = true;
          posq.pop();
          continue;
      }

      posq.push(posq.front()+TL);posq.push(posq.front()+Top);posq.push(posq.front()+RT);posq.push(posq.front()+Left);posq.push(posq.front()+Right);posq.push(posq.front()+LB);posq.push(posq.front()+Bottom);posq.push(posq.front()+BR);
      VisitedPos[posq.front().i][posq.front().j] = true;
      posq.pop();
    }

    return backNownedCavePos;
  }

  //Pos runAway(Pos current_pos, bool findWizard) {
  //}

  void updateWeightMap(MW &M, bool forWizard) {
    Cell c; Pos p; Unit u; WeightPos wp; int w = 0;
    for(p.i = 0; p.i < ROWS; p.i++) {
      for(p.j = 0; p.j < COLS; p.j++) {
        w = 0;
        c = cell(p);
        switch (c.type) {
        case Outside:
          if(forWizard) w += OUTSIDE_WEIGHT_WIZARD;
          else w += OUTSIDE_WEIGHT_DWARF;
          break;
        case Cave:
          if(forWizard) {
            if(c.treasure) w = TREASURE_WEIGHT_WIZARD;
            else if(c.owner == me()) w += CAVE_OWNED_WEIGHT_WIZARD;
            else w += CAVE_NOWNED_WEIGHT_WIZARD;
          }
          else {
            if(c.treasure) {w = TREASURE_WEIGHT_DWARF; treasuresLeft++;}
            else if(c.owner == me()) w += CAVE_OWNED_WEIGHT_DWARF;
            else w += CAVE_NOWNED_WEIGHT_DWARF;
          }
          break;
        case Abyss:
          w += ABYSS_WEIGHT;
          break;
        case Granite:
          w += GRANITE_WEIGHT;
          break;
        case Rock:
          if(forWizard) w += ROCK_WEIGHT_WIZARD;
          else w += (c.turns + 1);
          break;
        default:
          w += NONE_WEIGHT;
          break;
        }

        if(c.id != -1) {
          u = unit(c.id);
          if(u.player != me()) {
            switch (u.type) {
            case Dwarf:
              if(forWizard) w += DWARF_NOWNED_WEIGHT_WIZARD;
              else w += (u.health/30);
              break;
            case Wizard:
              if(forWizard) w += WIZARD_NOWNED_WEIGHT_WIZARD;
              else w += (u.health/30);
              break;
            case Orc:
              if(forWizard) w += ORC_WEIGHT_WIZARD;
              else w += (u.health/30);
              break;
            case Troll:
              w += TROLL_WEIGHT;
              break;
            case Balrog:
              w += BALROG_WEIGHT;
              break;
            default:
              w += NONE_WEIGHT;
              break;
            } 
          } else w += UNIT_OWNED_WEIGHT;
        }
        
        wp.p = p;
        wp.w = w;
        M[p.i][p.j] = wp;
      }
    }
  }

  void clearMW(MW &M) {
    int rsize = M.size();
    int csize = M[0].size();
    Pos tp;
    for(tp.i = 0; tp.i < rsize; tp.i++) {
      for(tp.j = 0; tp.j < csize; tp.j++) {
        M[tp.i][tp.j].w = NONE_WEIGHT;
        M[tp.i][tp.j].p = tp;
      }
    }     
  }

  VP findShortestRegularPath(Pos start_pos, Pos final_pos) {
    clearMB(VisitedPos);
    clearMW(WeightGraph);
    WeightPos cwp; cwp.p = start_pos; cwp.w = 0;
    WeightPos ewp; ewp.p = final_pos;
    WeightGraph[start_pos.i][start_pos.j] = cwp;
    Pos ctl;
    VP shortestPath;
    priority_queue<WeightPos> pq;
    pq.push(cwp);

    while(pq.size() > 0) {
      cwp = pq.top();
      pq.pop();
      if(!pos_ok(cwp.p)) continue; 
      VisitedPos[cwp.p.i][cwp.p.j] = true;
      
      ctl = cwp.p + TL;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Top;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + RT;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Left;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Right;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + LB;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Bottom;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + BR;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMap[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMap[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

    }

    while(ewp.p != start_pos) {
      shortestPath.insert(shortestPath.begin(), ewp.p);
      ewp = WeightGraph[ewp.b.i][ewp.b.j];
    }

    return shortestPath;
  }

  VP findShortestNonRegularPath(Pos start_pos, Pos final_pos) {
    clearMB(VisitedPos);
    clearMW(WeightGraph);
    WeightPos cwp; cwp.p = start_pos; cwp.w = 0;
    WeightPos ewp; ewp.p = final_pos;
    WeightGraph[start_pos.i][start_pos.j] = cwp;
    Pos ctl;
    VP shortestPath;
    priority_queue<WeightPos> pq;
    pq.push(cwp);

    while(pq.size() > 0) {
      cwp = pq.top();
      pq.pop();
      if(!pos_ok(cwp.p)) continue; 
      VisitedPos[cwp.p.i][cwp.p.j] = true;
      
      ctl = cwp.p + Top;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMapWizard[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMapWizard[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Left;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMapWizard[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMapWizard[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Right;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMapWizard[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMapWizard[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

      ctl = cwp.p + Bottom;
      if(pos_ok(ctl) && !VisitedPos[ctl.i][ctl.j]) {
        if(ctl == final_pos) {
          ewp.b = cwp.p;
          break;
        }
        if(cwp.w + WeightMapWizard[ctl.i][ctl.j].w < WeightGraph[ctl.i][ctl.j].w) {
          WeightGraph[ctl.i][ctl.j].w = cwp.w + WeightMapWizard[ctl.i][ctl.j].w;
          WeightGraph[ctl.i][ctl.j].b = cwp.p;
          pq.push(WeightGraph[ctl.i][ctl.j]);
        }
      }

    }

    while(ewp.p != start_pos) {
      shortestPath.insert(shortestPath.begin(), ewp.p);
      ewp = WeightGraph[ewp.b.i][ewp.b.j];
    }

    return shortestPath;
  }

  void deleteIthPos(VP &V, int pos) {
    int vsize = V.size();
    if (vsize < 1)
      return;
    for (int i = pos + 1; i < vsize; i++)
      V[i - 1] = V[i];
    V.pop_back();
  }

  void deleteFrontPos(VP &V) {
    deleteIthPos(V, 0);
  }

  bool followPath(VP &Path, int uid) {
    if (Path.size() < 1)
      return false;
    int vsize = Path.size();
    for (int i = 0; i < vsize; i++)
    {
      if (!pos_ok(Path[i]))
      {
        deleteIthPos(Path, i);
        i--;
        vsize--;
      }
    }
    if (Path.size() < 1)
      return false;

    Unit u = unit(uid);
    if (u.pos == Path[0])
      deleteFrontPos(Path);

    if (Path.size() < 1)
      return false;
    vsize = Path.size();
    for (int i = 0; i < vsize; i++)
    {
      if (!pos_ok(Path[i]))
      {
        deleteIthPos(Path, i);
        i--;
        vsize--;
      }
    }
    if (Path.size() < 1)
      return false;

    //printd("Unit " + to_string(u.id) + " " + to_string(u.pos.i) + "," + to_string(u.pos.j) + " moving " + dirTOstr(posDir(u.pos, Path[0])) + " towards " + to_string(Path[0].i) + "," + to_string(Path[0].j));

    command(uid, posDir(u.pos, Path[0]));
    return true;
  }

  // VPI checkPriorities(Unit u) {

  // }

  void updateMaxEnemyInfo() {
    maxEnemy = player1ID;
    maxEnemyCells = nb_cells(player1ID);
    if(nb_cells(player2ID) > maxEnemyCells) {
      maxEnemy = player2ID;
      maxEnemyCells = nb_cells(player2ID);
    }
    if(nb_cells(player3ID) > maxEnemyCells) {
      maxEnemy = player3ID;
      maxEnemyCells = nb_cells(player3ID);
    }
  }

  /**
   * Setup (First Round)
   */

  void setup() {
    VisitedPos = MB(60, VB(60, false));
    WeightMap = MW(60, VW(60));
    WeightMapWizard = MW(60, VW(60));
    WeightGraph = MW(60, VW(60));

    Dwarves = VU();
    wizardDirs = VD{Top, Left, Right, Bottom};

    VI tpid;
    if (0 != me())
      tpid.push_back(0);
    if (1 != me())
      tpid.push_back(1);
    if (2 != me())
      tpid.push_back(2);
    if (3 != me())
      tpid.push_back(3);
    player1ID = tpid[0];
    player2ID = tpid[1];
    player3ID = tpid[2];
    sauronID = -1;
  }

  /**
   * Setdown (Last Round)
   */
  void setdown() {
    // VI testvi {1111,2,3};
    // printVI(testvi);
    // VD testvd {Bottom, BR, Right, RT, Top, TL, Left, LB, None};
    // printVD(testvd);
    // Cell c; c.type = Cave; c.owner = me(); c.id = 22; c.turns = 4; c.treasure = true;
    // VC testvc {c, c, c, c};
    // printVC(testvc);
    // Unit u; u.type = Dwarf; u.id = 3; u.player = me(); u.health = 22; Pos p; p.i = 7; p.j = 11; u.pos = p;
    // VU testvu {u, u, u, u};
    // printVU(testvu);
    // printVC(TrackedCells);
    //Unit fdwa = unit(dwarves(me())[0]);
    //printd("Searching enemy near " + to_string(fdwa.pos.i) + "," + to_string(fdwa.pos.j));
    //Pos tsp = findEnemy(fdwa.pos, -1, -1);
    //printd("Found enemy at " + to_string(tsp.i) + "," + to_string(tsp.j));
    //tsp = findNownedCaves(fdwa.pos, player1ID, false);
    //printd("Found no owned cell at " + to_string(tsp.i) + "," + to_string(tsp.j));
    //Unit fwiz = unit(wizards(me())[0]);
    //printd("Searching ally near " + to_string(fwiz.pos.i) + "," + to_string(fwiz.pos.j));
    //Pos tspw = findAlly(fwiz.pos, -1);
    //printd("Found ally at " + to_string(tspw.i) + "," + to_string(tspw.j));
    // string tmpstr;
    // for(int i = 0; i < 10; i++) {
    //   tmpstr = "";
    //   for(int j = 0; j < 40; j++) {
    //     tmpstr += "(" + to_string(i) + "," + to_string(j) + " [" + to_string(WeightMap[i][j].w) + "]) ";
    //   }
    //   printd(tmpstr);
    // }
    //Pos s; s.i = 0; s.j = 0;
    //Pos e; e.i = 7; e.j = 18;
    //VP shortest = findShortestRegularPath(s,e);
    //printVP(shortest);
  }

  /**
   * Decision Functions
   */

  //void updateDwarves(VU &V) {

  //}

  void moveDwarves() {
    VI dwarvesIDs = dwarves(me());
    VU dwarvesUnits = viTOvu(dwarvesIDs);
    int dwusize = dwarvesUnits.size();
    VP pat;

    for(int i = 0; i < dwusize; i++) {
      if(dwusize < MIN_NUM_CLAN_DWARVES && round() < MAX_ROUND_ATTACK) pat = findShortestRegularPath(dwarvesUnits[i].pos, findEnemy(dwarvesUnits[i].pos, -1, -1));
      else if(treasuresLeft > MAX_TREASURES_LEFT) pat = findShortestRegularPath(dwarvesUnits[i].pos, findTreasures(dwarvesUnits[i].pos, -1, false));
      else if(maxEnemyCells >= MAX_ENEMY_CELLS) pat = findShortestRegularPath(dwarvesUnits[i].pos, findNownedCaves(dwarvesUnits[i].pos, maxEnemy, false));
      else pat = findShortestRegularPath(dwarvesUnits[i].pos, findNownedCaves(dwarvesUnits[i].pos, -1, false));

      //if(treasuresLeft > MAX_TREASURES_LEFT) pat = findShortestRegularPath(dwarvesUnits[i].pos, findTreasures(dwarvesUnits[i].pos, -1, false));
      //else pat = findShortestRegularPath(dwarvesUnits[i].pos, findEnemy(dwarvesUnits[i].pos, -1, -1));

      followPath(pat, dwarvesUnits[i].id);
    }

  }

  void moveWizards() {
    VI wizardsIDs = wizards(me());
    VU wizardsUnits = viTOvu(wizardsIDs);
    for (Unit Wizard : wizardsUnits) {
      VP pat = findShortestNonRegularPath(Wizard.pos, findAlly(Wizard.pos,-1));
      if(!followPath(pat, Wizard.id)) {
        command(Wizard.id, randDir(wizardDirs));
      }
    }
  }

  //void updateDwarvesPaths() {

  //}

  //void updateWizardsPaths() {

  //}

  /**
   * Play method, invoked once per each round.
   */
  virtual void play() {
    if(status(me()) >= 0.99) { 
      for(int Dwarf : dwarves(me())) command(Dwarf, Dir(random(0,7)));
      for(int Wizard : wizards(me())) command(Wizard, randDir(wizardDirs));
      return;
    }
    if (round() == 0) setup();
    printd(logMSG());

    updateWeightMap(WeightMap, false);
    updateWeightMap(WeightMapWizard, true);
    updateMaxEnemyInfo();

    // ========
    //updateDwarvesPaths();
    //updateWizardsPaths();

    moveDwarves();
    moveWizards();
    // ========

    treasuresLeft = 0;

    printd("===================");
    if (round() == 199) setdown();
  }
};

/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
