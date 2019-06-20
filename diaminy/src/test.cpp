
void assert_true(bool b, const char *message) {
    if (!b) {
        throw assertion_error(message);
    }
}

void assert_false(bool b, const char *message) {
    assert_true(!b, message);
}

void test_gamestate1() {
    GameState first = {
            {1, 2, 3},
            {UP, UP},
            0
    };
    GameState second = {
            {1, 2, 3},
            {UP, UP},
            0
    };

    assert_false(first.is_worse_than(second), "first < second 1");
    assert_false(second.is_worse_than(first), "first > second 1");
}

void test_gamestate2() {
    GameState first = {
            {1, 2},
            {UP, UP},
            0
    };
    GameState second = {
            {1, 2, 3},
            {UP, UP},
            0
    };

    assert_true(first.is_worse_than(second), "first < second 2");
    assert_false(second.is_worse_than(first), "first > second 2");
}

void test_gamestate3() {
    GameState first = {
            {1, 2},
            {UP, UP, DOWN},
            0
    };
    GameState second = {
            {1, 2, 3},
            {UP, UP},
            0
    };

    assert_true(first.is_worse_than(second), "first < second 3");
    assert_false(second.is_worse_than(first), "first > second 3");
}

void test_gamestate4() {
    GameState first = {
            {1, 2},
            {UP},
            0
    };
    GameState second = {
            {1, 2, 3},
            {UP, UP},
            0
    };

    assert_false(first.is_worse_than(second), "first < second 3");
    assert_false(second.is_worse_than(first), "first > second 3");
}

int test() {
    try {
        test_gamestate1();
        test_gamestate2();
        test_gamestate3();
        test_gamestate4();
    } catch (assertion_error &e) {
        std::cout << "Assertion failed: " << e.what() << '\n';
    }
    return 0;
}
