#include <superkarel.h>

void turn_around(void) {
    turn_left();
    turn_left();
}

void turn_right(void) {
    turn_left();
    turn_left();
    turn_left();
}

void turn_to_north(void) {
    while (!facing_north()) {
        turn_left();
    }
}

void turn_to_east(void) {
    while (!facing_east()) {
        turn_left();
    }
}

void turn_to_south(void) {
    while (!facing_south()) {
        turn_left();
    }
}

void turn_to_west(void) {
    while (!facing_west()) {
        turn_left();
    }
}

bool has_extra_marker(void) {
    pick_beeper();
    pick_beeper();

    if (beepers_present()) {
        put_beeper();
        put_beeper();
        return true;
    }

    put_beeper();
    put_beeper();
    return false;
}

bool other_entrance_south(void) {
    turn_to_south();

    if (!front_is_clear()) {
        turn_to_north();
        return false;
    }

    step();
    if (no_beepers_present()) {
        turn_to_north();
        step();
        turn_to_north();
        return false;
    }

    pick_beeper();
    if (no_beepers_present()) {
        put_beeper();
        turn_to_north();
        step();
        turn_to_north();
        return false;
    }

    pick_beeper();
    if (no_beepers_present()) {
        put_beeper();
        put_beeper();
        turn_to_north();
        step();
        turn_to_north();
        return true;
    }

    put_beeper();
    put_beeper();
    turn_to_north();
    step();
    turn_to_north();
    return false;
}

bool search_room(void);
void mark_other_doors(void);

bool search_north(void) {
    turn_to_north();
    if (front_is_clear()) {
        step();
        if (no_beepers_present() && search_room()) {
            turn_to_south();
            step();
            turn_to_north();
            return true;
        }
        turn_to_south();
        step();
        turn_to_north();
    }
    return false;
}

bool search_east(void) {
    turn_to_east();
    if (front_is_clear()) {
        step();
        if (no_beepers_present() && search_room()) {
            turn_to_west();
            step();
            turn_to_north();
            return true;
        }
        turn_to_west();
        step();
    }
    turn_to_north();
    return false;
}

bool search_south(void) {
    turn_to_south();
    if (front_is_clear()) {
        step();
        if (no_beepers_present() && search_room()) {
            turn_to_north();
            step();
            turn_to_north();
            return true;
        }
        turn_to_north();
        step();
    }
    turn_to_north();
    return false;
}

bool search_west(void) {
    turn_to_west();
    if (front_is_clear()) {
        step();
        if (no_beepers_present() && search_room()) {
            turn_to_east();
            step();
            turn_to_north();
            return true;
        }
        turn_to_east();
        step();
    }
    turn_to_north();
    return false;
}

bool search_room(void) {
    put_beeper();

    if (other_entrance_south()) {
        pick_beeper();
        return true;
    }
    if (search_north()) {
        pick_beeper();
        return true;
    }
    if (search_east()) {
        pick_beeper();
        return true;
    }
    if (search_south()) {
        pick_beeper();
        return true;
    }
    if (search_west()) {
        pick_beeper();
        return true;
    }

    pick_beeper();
    return false;
}

void mark_north(void) {
    turn_to_north();
    if (front_is_clear()) {
        step();
        if (no_beepers_present()) {
            mark_other_doors();
        }
        turn_to_south();
        step();
        turn_to_north();
    }
}

void mark_east(void) {
    turn_to_east();
    if (front_is_clear()) {
        step();
        if (no_beepers_present()) {
            mark_other_doors();
        }
        turn_to_west();
        step();
        turn_to_north();
    }
}

void mark_south(void) {
    turn_to_south();
    if (front_is_clear()) {
        step();
        if (no_beepers_present()) {
            mark_other_doors();
        }
        turn_to_north();
        step();
        turn_to_north();
    }
}

void mark_west(void) {
    turn_to_west();
    if (front_is_clear()) {
        step();
        if (no_beepers_present()) {
            mark_other_doors();
        }
        turn_to_east();
        step();
        turn_to_north();
    }
}

void mark_other_doors(void) {
    put_beeper();

    if (other_entrance_south()) {
        turn_to_south();
        step();
        put_beeper();
        turn_to_north();
        step();
        turn_to_north();
    }

    mark_north();
    mark_east();
    mark_south();
    mark_west();

    pick_beeper();
}

void process_current_corridor_cell(void) {
    turn_left();

    if (front_is_clear() && !has_extra_marker()) {
        put_beeper();
        step();

        if (search_room()) {
            mark_other_doors();
            turn_around();
            step();
            turn_around();
        } else {
            turn_around();
            step();
            pick_beeper();
            turn_around();
        }
    }

    turn_right();
}

void clean_corridor_cell(void) {
    if (has_extra_marker()) {
        turn_left();
        step();
        put_beeper();
        turn_around();
        step();
        turn_left();
        pick_beeper();
        pick_beeper();
        pick_beeper();
        return;
    }

    pick_beeper();
    pick_beeper();
}

int main(void) {
    turn_on("task_7.kw");
    set_step_delay(50);

    put_beeper();
    put_beeper();
    while (front_is_clear()) {
        step();
        put_beeper();
        put_beeper();
    }

    turn_around();
    while (front_is_clear()) {
        step();
    }
    turn_around();

    process_current_corridor_cell();
    while (front_is_clear()) {
        step();
        process_current_corridor_cell();
    }

    turn_around();
    while (front_is_clear()) {
        step();
    }
    turn_around();

    clean_corridor_cell();
    while (front_is_clear()) {
        step();
        clean_corridor_cell();
    }

    turn_off();
    return 0;
}
