//
// Created by thecharlesblake on 12/26/17.
//

#include <rapidjson/document.h>
#include <gtest/gtest.h>

#include "test_helper.h"
#include "../main/game/game_state.h"
#include "../main/solver/solver.h"
#include "../main/input-output/input/json-parsing/json_helper.h"
#include "../main/input-output/input/json-parsing/rules_parser.h"

#define ASSERT_(t_f, statement) ASSERT_TRUE((statement) == (t_f))

using namespace rapidjson;
using namespace std;

bool test_helper::is_solvable(const std::string& input_file, const std::string& preset_type) {
    const Document in_doc = json_helper::get_file_json(input_file);
    const sol_rules rules = rules_parser::from_preset(preset_type);

    game_state gs(rules, in_doc);
    solver sol(gs);

    return sol.run();
}

void test_helper::run_foundations_dominance_test(sol_rules::build_policy policy,
                                                 std::vector<card> cards) {
    sol_rules rules = rules_parser::from_preset("simple-free-cell");
    rules.build_pol = policy;
    rules.foundations_removable = false;
    rules.cells = 0;

    game_state gs = game_state(rules, std::initializer_list<pile>{
            {},
            {},
            {},
            {},
            {"4C","3C","2C","AC"},
            {"4H","3H","2H","AH"},
            {"4S","3S","2S","AS"},
            {"4D","3D","2D","AD"}
    });

    solver sol = solver(game_state(gs));
    sol.run();

    solver::node const *n = &sol.get_search_tree();
    for (card c : cards) {
        n = &n->children.back();
        ASSERT_TRUE(n->move.is_dominance());
        ASSERT_TRUE(gs.get_data()[n->move.from].top_card() == c);
        ASSERT_TRUE(n->move.to >= 0 && n->move.to <= 4);
        gs.make_move(n->move);
    }
    ASSERT_TRUE(n->children.empty());
}