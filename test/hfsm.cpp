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
#include "collections.h"
#include "hfsm.h"
}

using Catch::Matchers::Equals;

FSM_STATE(state_root_with_no_handler, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler2, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler3, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler4, NULL, NULL, NULL, NULL);
FSM_STATE(state_root_with_no_handler5, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_3, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_1_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_1_2, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_2, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_3, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_4, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_3_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_1_1_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_1_2_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_1_2_2, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_2_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_2_1_1, NULL, NULL, NULL, NULL);
FSM_STATE(state_nested_2_2_1_1_1, NULL, NULL, NULL, NULL);

static bool entry_only_param = false;
static void entry_only_entry(struct fsm *machine, void *data, bool cmpl)
{
    bool *called = (bool *)data;
    *called = true;
}
FSM_STATE(state_entry_only, &entry_only_param, entry_only_entry, NULL, NULL);

FSM_EVENT(event_1);
FSM_EVENT(event_2);
FSM_EVENT(event_3);
FSM_EVENT(event_4);
FSM_EVENT(event_5);

static bool cond_only_param = false;
FSM_COND(cond_only_cond, (struct fsm *machine))
{
    return cond_only_param;
}

static bool action_only_param = false;
FSM_ACTION(action_only_action, (struct fsm *machine))
{
    action_only_param = true;
}

SCENARIO("状態マシンが初期化できること", "[fsm][init]") {
    GIVEN("特になし") {
        WHEN("状態遷移なしで状態マシンを初期化する") {
            struct fsm *machine = fsm_init(NULL, NULL);

            THEN("戻り値が NULL となること") {
                REQUIRE(machine == NULL);
            }

            fsm_term(machine);
        }

        WHEN("1 つの状態遷移で状態マシンを初期化する") {
            const struct fsm_trans corresps[] = {
                FSM_TRANS_HELPER(state_start, event_1, NULL, NULL, state_root_with_no_handler),
                FSM_TRANS_TERMINATOR
            };
            struct fsm *machine = fsm_init(NULL, corresps);

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
            FSM_TRANS_HELPER(state_start, event_1, NULL, NULL, state_root_with_no_handler),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを発生させる") {
            fsm_transition(machine, event_1);

            THEN("現在の状態が 'state_root_with_no_handler' になること") {
                char name[32] = {0};
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler"));
            }
        }

        WHEN("状態遷移にないイベントを発生させる") {
            fsm_transition(machine, event_5);

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
            FSM_TRANS_HELPER(state_start, event_1, NULL, NULL, state_root_with_no_handler),
            FSM_TRANS_HELPER(state_root_with_no_handler2, event_3, NULL, NULL, state_root_with_no_handler3),
            FSM_TRANS_HELPER(state_root_with_no_handler4, event_5, NULL, NULL, state_root_with_no_handler5),
            FSM_TRANS_HELPER(state_root_with_no_handler, event_2, NULL, NULL, state_root_with_no_handler2),
            FSM_TRANS_HELPER(state_root_with_no_handler3, event_4, NULL, NULL, state_root_with_no_handler4),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを順に発生させる") {
            THEN("最終的に現在の状態が 'state_root_with_no_handler5' になること") {
                char name[32] = {0};

                fsm_transition(machine, event_1);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler"));

                fsm_transition(machine, event_2);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler2"));

                fsm_transition(machine, event_3);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler3"));

                fsm_transition(machine, event_4);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler4"));

                fsm_transition(machine, event_5);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler5"));
            }
        }

        fsm_term(machine);
    }

    GIVEN("Null 遷移を含む 5 つの状態遷移で状態マシンを初期化する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(state_start, event_null, NULL, NULL, state_root_with_no_handler),
            FSM_TRANS_HELPER(state_root_with_no_handler2, event_null, NULL, NULL, state_root_with_no_handler3),
            FSM_TRANS_HELPER(state_root_with_no_handler4, event_null, NULL, NULL, state_root_with_no_handler5),
            FSM_TRANS_HELPER(state_root_with_no_handler, event_1, NULL, NULL, state_root_with_no_handler2),
            FSM_TRANS_HELPER(state_root_with_no_handler3, event_2, NULL, NULL, state_root_with_no_handler4),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを順に発生させる") {
            THEN("最終的に現在の状態が 'state_root_with_no_handler5' になること") {
                char name[32] = {0};

                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler"));

                fsm_transition(machine, event_1);
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler3"));

                fsm_transition(machine, event_2);
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
            FSM_TRANS_HELPER(state_start, event_1, NULL, NULL, state_entry_only),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを発生させる") {
            REQUIRE(entry_only_param == false);
            fsm_transition(machine, event_1);

            THEN("entry アクションが呼び出されること") {
                REQUIRE(entry_only_param == true);
            }
        }

        fsm_term(machine);
    }

    GIVEN("イベントアクションありの状態遷移を定義する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(state_start, event_1, NULL, action_only_action, state_root_with_no_handler),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移にあるイベントを発生させる") {
            REQUIRE(action_only_param == false);
            fsm_transition(machine, event_1);

            THEN("イベントアクションが呼び出されること") {
                REQUIRE(action_only_param == true);
            }
        }

        fsm_term(machine);
    }
}

SCENARIO("ガード条件により遷移がキャンセル中止されること", "[fsm][cond]") {
    GIVEN("ガード条件ありの遷移を定義する") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(state_start, event_1, cond_only_cond, NULL, state_root_with_no_handler),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("ガード条件を満たす状態で遷移を行う") {
            cond_only_param = true;
            fsm_transition(machine, event_1);

            THEN("状態遷移が行われること") {
                char name[32] = {0};
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("state_root_with_no_handler"));
            }
        }

        WHEN("ガード条件を満たさない状態で遷移を行う") {
            cond_only_param = false;
            fsm_transition(machine, event_1);

            THEN("状態遷移が行われないこと") {
                char name[32] = {0};
                fsm_current_state(machine, name, sizeof(name));
                REQUIRE_THAT(name, Equals("start"));
            }
        }


        fsm_term(machine);
    }
}

SCENARIO("状態遷移の定義をダンプする", "[fsm][dump]") {
    GIVEN("1 つの状態から複数の遷移がある定義を行う") {
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(state_start, event_null, NULL, NULL, state_nested_1),
            FSM_TRANS_HELPER(state_start, event_1, NULL, NULL, state_nested_2),
            FSM_TRANS_HELPER(state_start, event_2, NULL, NULL, state_nested_3),
            FSM_TRANS_HELPER(state_nested_1, event_3, NULL, NULL, state_nested_2),
            FSM_TRANS_HELPER(state_nested_1, event_4, NULL, NULL, state_nested_3),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(NULL, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移をダンプする") {
            static LIST list;

            class handler {
                public: static void run(TREE tree) {
                    TREE_ITER iter;


                    for (iter = tree_iter_get(tree); iter != NULL; iter = tree_iter_next(iter)) {
                        const struct fsm_state *state =
                            *(const struct fsm_state **)tree_iter_get_payload(iter);
                        list_add(list, (void *)&state);
                    }

                    tree_iter_release(iter);
                }
            };

            list = list_init(sizeof(struct fsm_state *), 30);
            fsm_dump_state_transition(machine, handler::run);

            THEN("定義内容と一致すること") {
                ITER iter = list_iter(list);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_start);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_3);
            }
        }

        fsm_term(machine);
    }

    GIVEN("ネストした状態の遷移がある定義を行う") {
        const struct fsm_rels rels[] = {
            FSM_RELS_HELPER(state_nested_1_1, state_nested_1, true),
            FSM_RELS_HELPER(state_nested_1_2, state_nested_1, false),
            FSM_RELS_HELPER(state_nested_2_1, state_nested_2, true),
            FSM_RELS_HELPER(state_nested_2_2, state_nested_2, false),
            FSM_RELS_HELPER(state_nested_2_3, state_nested_2, false),
            FSM_RELS_HELPER(state_nested_2_4, state_nested_2, false),
            FSM_RELS_HELPER(state_nested_3_1, state_nested_3, true),
            FSM_RELS_HELPER(state_nested_1_1_1, state_nested_1_1, true),
            FSM_RELS_HELPER(state_nested_1_2_1, state_nested_1_2, true),
            FSM_RELS_HELPER(state_nested_1_2_2, state_nested_1_2, false),
            FSM_RELS_HELPER(state_nested_2_2_1, state_nested_2_2, true),
            FSM_RELS_HELPER(state_nested_2_2_1_1, state_nested_2_2_1, true),
            FSM_RELS_HELPER(state_nested_2_2_1_1_1, state_nested_2_2_1_1, true),
            FSM_RELS_TERMINATOR
        };
        const struct fsm_trans corresps[] = {
            FSM_TRANS_HELPER(state_start, event_null, NULL, NULL, state_nested_1),
            FSM_TRANS_HELPER(state_start, event_1, NULL, NULL, state_nested_2),
            FSM_TRANS_HELPER(state_start, event_2, NULL, NULL, state_nested_3),
            FSM_TRANS_HELPER(state_nested_1, event_3, NULL, NULL, state_nested_2),
            FSM_TRANS_HELPER(state_nested_1, event_4, NULL, NULL, state_nested_3),
            FSM_TRANS_HELPER(state_nested_1, event_1, NULL, NULL, state_nested_2_1),
            FSM_TRANS_HELPER(state_nested_1, event_2, NULL, NULL, state_nested_2_2),
            FSM_TRANS_HELPER(state_nested_1, event_5, NULL, NULL, state_nested_2_2_1_1_1),
            FSM_TRANS_HELPER(state_nested_2, event_3, NULL, NULL, state_nested_1_1),
            FSM_TRANS_HELPER(state_nested_2, event_4, NULL, NULL, state_nested_1_2),
            FSM_TRANS_HELPER(state_nested_2, event_5, NULL, NULL, state_nested_3),
            FSM_TRANS_HELPER(state_nested_1_1, event_1, NULL, NULL, state_nested_2_3),
            FSM_TRANS_HELPER(state_nested_1_1, event_2, NULL, NULL, state_nested_2_4),
            FSM_TRANS_HELPER(state_nested_2_1, event_1, NULL, NULL, state_nested_3_1),
            FSM_TRANS_HELPER(state_nested_2_1, event_2, NULL, NULL, state_nested_1_1_1),
            FSM_TRANS_HELPER(state_nested_2_1, event_3, NULL, NULL, state_nested_1_2_1),
            FSM_TRANS_HELPER(state_nested_2_2, event_1, NULL, NULL, state_nested_1_2_2),
            FSM_TRANS_HELPER(state_nested_2_2, event_2, NULL, NULL, state_nested_1_1),
            FSM_TRANS_HELPER(state_nested_3_1, event_1, NULL, NULL, state_nested_1_1),
            FSM_TRANS_HELPER(state_nested_2_2_1_1_1, event_1, NULL, NULL, state_end),
            FSM_TRANS_TERMINATOR
        };
        struct fsm *machine = fsm_init(rels, corresps);
        REQUIRE(machine != NULL);

        WHEN("状態遷移をダンプする") {
            static LIST list;

            class handler {
                public: static void run(TREE tree) {
                    TREE_ITER iter;

                    for (iter = tree_iter_get(tree); iter != NULL; iter = tree_iter_next(iter)) {
                        const struct fsm_state *state =
                            *(const struct fsm_state **)tree_iter_get_payload(iter);
                        list_add(list, (void *)&state);
                    }

                    tree_iter_release(iter);
                }
            };

            list = list_init(sizeof(struct fsm_state *), 30);
            fsm_dump_state_transition(machine, handler::run);

            THEN("定義内容と一致すること") {
                ITER iter = list_iter(list);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_start);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1_1_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1_2);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1_2_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_1_2_2);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_2);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_2_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_2_1_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_2_1_1_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_3);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_2_4);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_3);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_nested_3_1);
                iter = iter_next(iter);
                REQUIRE(*(const struct fsm_state **)iter_get_payload(iter) == state_end);
            }

            list_release(list);
        }

        fsm_term(machine);
    }
}
