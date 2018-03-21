/** @file   hfsm.cpp
 *  @brief  階層型有限状態マシンのテスト.
 *
 *  @author t-kenji <protect.2501@gmail.com>
 *  @date   2018-03-18 新規作成.
 */
#include <cstdio>

#include <catch.hpp>

extern "C" {
#include "debug.h"
#include "hfsm.h"
}

using Catch::Matchers::Equals;

FSM_STATE(state_root_with_no_handler, NULL, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler2, NULL, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler3, NULL, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler4, NULL, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler5, NULL, NULL, NULL, NULL, NULL);

static bool entry_only_param = false;
static void entry_only_entry(struct fsm *machine, void *data, bool cmpl)
{
    bool *called = (bool *)data;
    *called = true;
}
FSM_STATE(state_entry_only, NULL, &entry_only_param, entry_only_entry, NULL, NULL);

static bool action_only_param = false;
static void action_only_action(struct fsm *machine)
{
    action_only_param = true;
}

SCENARIO("状態マシンが初期化できること", "[fsm][init]") {
    GIVEN("特になし") {
        WHEN("状態遷移なしで状態マシンを初期化する") {
            struct fsm *machine = fsm_init(NULL);

            THEN("戻り値が NULL となること") {
                REQUIRE(machine == NULL);
            }

            fsm_term(machine);
        }

        WHEN("1 つの状態遷移で状態マシンを初期化する") {
            const struct fsm_trans corresps[] = {
                FSM_TRANS_HELPER(&state_start, 0, NULL, &state_root_with_no_handler),
                FSM_TRANS_TERMINATOR
            };
            struct fsm *machine = fsm_init(corresps);

            THEN("戻り値が非 NULL となること") {
                REQUIRE(machine != NULL);
            }

            fsm_term(machine);
        }
    }
}

SCENARIO("状態遷移できること", "[fsm][trans]") {
    GIVEN("1 つの状態遷移で状態マシンを初期化する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(&state_start, 0, NULL, &state_root_with_no_handler),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを発生させる") {
            fsm_transition(machine, 0);

            THEN("現在の状態が 'state_root_with_no_handler' になること") {
                char name[32] = {0};
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler"));
            }
        }

        WHEN("状態遷移にないイベントを発生させる") {
            fsm_transition(machine, 9);

            THEN("現在の状態が 'start' のままであること") {
                char name[32] = {0};
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("start"));
            }
        }


        fsm_term(machine);
    }

    GIVEN("5 つの状態遷移で状態マシンを初期化する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(&state_start, 0, NULL, &state_root_with_no_handler),
            FSM_TRANS_HELPER(&state_root_with_no_handler2, 2, NULL, &state_root_with_no_handler3),
            FSM_TRANS_HELPER(&state_root_with_no_handler4, 4, NULL, &state_root_with_no_handler5),
            FSM_TRANS_HELPER(&state_root_with_no_handler, 1, NULL, &state_root_with_no_handler2),
            FSM_TRANS_HELPER(&state_root_with_no_handler3, 3, NULL, &state_root_with_no_handler4),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを順に発生させる") {
            THEN("最終的に現在の状態が 'state_root_with_no_handler5' になること") {
                char name[32] = {0};

                fsm_transition(machine, 0);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler"));

                fsm_transition(machine, 1);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler2"));

                fsm_transition(machine, 2);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler3"));

                fsm_transition(machine, 3);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler4"));

                fsm_transition(machine, 4);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler5"));
            }
        }

        fsm_term(machine);
    }
}

SCENARIO("遷移時にアクションが呼び出されること", "[fsm][action]") {
    GIVEN("entry アクションのみの状態への遷移を定義する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(&state_start, 0, NULL, &state_entry_only),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを発生させる") {
            REQUIRE(entry_only_param == false);
            fsm_transition(machine, 0);

            THEN("entry アクションが呼び出されること") {
                REQUIRE(entry_only_param == true);
            }
        }

        fsm_term(machine);
    }

    GIVEN("イベントアクションありの状態遷移を定義する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(&state_start, 0, action_only_action, &state_root_with_no_handler),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを発生させる") {
            REQUIRE(action_only_param == false);
            fsm_transition(machine, 0);

            THEN("イベントアクションが呼び出されること") {
                REQUIRE(action_only_param == true);
            }
        }

        fsm_term(machine);
    }
}
