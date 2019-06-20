
void assert_true(bool b, const char *message) {
    if (!b) {
        throw assertion_error(message);
    }
}

void assert_false(bool b, const char *message) {
    assert_true(!b, message);
}

std::shared_ptr<std::set<idx_t>> make_diamonds(std::initializer_list<idx_t> i) {
    return std::make_shared<std::set<idx_t>>(std::set<idx_t>(i));
}

// two identical states
void test_gamestate1() {
    GameState first = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_false(first.is_worse_than(second), "first < second 1");
    assert_false(second.is_worse_than(first), "first > second 1");
}

// differ in diamonds
void test_gamestate2() {
    GameState first = {
            make_diamonds({1, 2}),
            {UP, UP},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_true(first.is_worse_than(second), "first < second 2");
    assert_false(second.is_worse_than(first), "first > second 2");
}

// differ in diamonds and length
void test_gamestate3() {
    GameState first = {
            make_diamonds({1, 2}),
            {UP, UP, DOWN},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_true(first.is_worse_than(second), "first < second 3");
    assert_false(second.is_worse_than(first), "first > second 3");
}

// differ in diamonds and length 2
void test_gamestate4() {
    GameState first = {
            make_diamonds({1, 2}),
            {UP},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_false(first.is_worse_than(second), "first < second 4");
    assert_false(second.is_worse_than(first), "first > second 4");
}

// differ in length
void test_gamestate5() {
    GameState first = {
            make_diamonds({1, 2, 3}),
            {UP, DOWN, LEFT},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_true(first.is_worse_than(second), "first < second 5");
    assert_false(second.is_worse_than(first), "first > second 5");
}

// differ in diamonds
void test_gamestate6() {
    GameState first = {
            make_diamonds({1, 2, 4}),
            {UP, DOWN},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_false(first.is_worse_than(second), "first < second 6");
    assert_false(second.is_worse_than(first), "first > second 6");
}

// differ in path
void test_gamestate7() {
    GameState first = {
            make_diamonds({1, 2, 3}),
            {UP, DOWN},
            0
    };
    GameState second = {
            make_diamonds({1, 2, 3}),
            {UP, UP},
            0
    };

    assert_true(first.is_worse_than(second), "first < second 7");
    assert_false(second.is_worse_than(first), "first > second 7");
}

int test() {
    try {
        test_gamestate1();
        test_gamestate2();
        test_gamestate3();
        test_gamestate4();
        test_gamestate5();
        test_gamestate6();
        test_gamestate7();
    } catch (assertion_error &e) {
        std::cout << "Assertion failed: " << e.what() << '\n';
    }
    return 0;
}
