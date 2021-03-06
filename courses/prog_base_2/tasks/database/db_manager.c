#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sqlite3.h>

#include "db_manager.h"
#include "pupils.h"

struct db_s {
    sqlite3 * db;
};

db_t * db_new(const char * dbFile) {
    db_t * self = malloc(sizeof(struct db_s));
    int rc = sqlite3_open(dbFile, & self->db);
    if (SQLITE_ERROR == rc) {
        printf("can't open database\n");
        exit(1);
    }
    return self;
}

void db_free(db_t * self) {
    sqlite3_close(self->db);
    free(self);
}

static void _fillPupil(sqlite3_stmt * stmt, pupils_t * pupil) {
    int id = sqlite3_column_int(stmt, 0);
    const unsigned char * name = sqlite3_column_text(stmt, 1);
    const unsigned char * surname = sqlite3_column_text(stmt, 2);
    const unsigned char * birthdate = sqlite3_column_text(stmt, 3);
    int form = sqlite3_column_int(stmt, 4);
    double score = sqlite3_column_double(stmt, 5);
    pupil->id = id;
    strcpy(pupil->name, (const char *)name);
    strcpy(pupil->surname, (const char *)surname);
    strcpy(pupil->birthDate, (const char *)birthdate);
    pupil->form = form;
    pupil->score = score;
}

int db_insertPupil(db_t * self, pupils_t * pupils) {
    sqlite3_stmt * stmt =  NULL;

    int last = -1;
    const char * sql = "INSERT INTO PUPILS (Name, Surname, BirthDate, Form, Score) VALUES (?, ?, ?, ?, ?);";
    int rc = sqlite3_prepare_v2(self->db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, pupils->name, strlen(pupils->name), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pupils->surname, strlen(pupils->surname), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pupils->birthDate, strlen(pupils->birthDate), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, pupils->form);
    sqlite3_bind_double(stmt, 5, pupils->score);
    if(SQLITE_OK != rc) {
        printf("Not prepared!\n");
    }
    if (SQLITE_DONE != (rc = sqlite3_step(stmt))) {
        printf("Not inserted! %i\n", rc);
    }
    last = (int) sqlite3_last_insert_rowid (self->db);
    sqlite3_finalize(stmt);
    return last;
}

pupils_t * db_getPupilById(db_t * self, int id) {
    pupils_t * pupil = malloc(sizeof(struct pupils_s));
    sqlite3_stmt * stmt =  NULL;

    const char * sql = "SELECT * FROM pupils WHERE ID = ?";

    sqlite3_prepare_v2(self->db, sql, strlen(sql), &stmt, 0);
    sqlite3_bind_int(stmt, 1, id);
        int rc = sqlite3_step(stmt);
        if (SQLITE_ERROR == rc) {
            printf("can't select pupil!\n");
            exit(1);
        } else {
            _fillPupil(stmt, pupil);
        }
        sqlite3_finalize(stmt);
    return pupil;
}

void db_updatePupil(db_t * self, pupils_t * pupils, int id) {
    sqlite3_stmt * stmt =  NULL;
    const char * sql = "UPDATE PUPILS SET Name = ?, Surname = ?, BirthDate = ?, Form = ?, Score = ? WHERE ID = ?";
    int rc = sqlite3_prepare_v2(self->db, sql, strlen(sql), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, pupils->name, strlen(pupils->name), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pupils->surname, strlen(pupils->surname), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, pupils->birthDate, strlen(pupils->birthDate), SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, pupils->form);
    sqlite3_bind_double(stmt, 5, pupils->score);
    sqlite3_bind_int(stmt, 6, id);
    if(SQLITE_OK != rc) {
        printf("Not prepared!\n");
    }
    if (SQLITE_DONE != (rc = sqlite3_step(stmt))) {
        printf("Not Updated! %i\n", rc);
    } else {
        printf("Successfully updated");
    }
    sqlite3_finalize(stmt);
}

void db_deletePupil(db_t * self, int id) {
    sqlite3_stmt * stmt =  NULL;
    const char * sqlDeleteTest = "DELETE FROM PUPILS WHERE ID = ?";
    int rc = sqlite3_prepare_v2(self->db, sqlDeleteTest, strlen(sqlDeleteTest) + 1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);
    if(SQLITE_OK != rc) {
        printf("Not prepared!\n");
    }
    if (SQLITE_DONE != (rc = sqlite3_step(stmt))) {
        printf("Not deleted! %i\n", rc);
    } else {
    printf("Has been deleted!\n");
    }
    sqlite3_finalize(stmt);
}

int db_countPupils(db_t * self) {
    sqlite3_stmt * stmt = NULL;
    sqlite3_prepare_v2(self->db, "SELECT COUNT(*) FROM PUPILS;", -1, &stmt, 0);
    int rc = sqlite3_step(stmt);
    if (SQLITE_ERROR == rc) {
        printf("can't select count\n");
        exit(1);
    }
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count;
}

int db_getPupilsTask(db_t * self, int K, int P, pupils_t * arr) {
    sqlite3_stmt * stmt = NULL;
    const char * sqlQuery = "SELECT * FROM PUPILS WHERE form  = ? ORDER BY Score DESC limit ?";
    sqlite3_prepare_v2(self->db, sqlQuery, strlen(sqlQuery), &stmt, 0);
    sqlite3_bind_int(stmt, 1, P);
    sqlite3_bind_int(stmt, 2, K);
    int count = 0;
    while (1) {
        int rc = sqlite3_step(stmt);
        if (SQLITE_ERROR == rc) {
            printf("can't select pupils\n");
            exit(1);
        } else if (SQLITE_DONE == rc) {
            break;
        } else {
            _fillPupil(stmt, &arr[count]);
            count++;
        }
    }
    sqlite3_finalize(stmt);
    return count;
}

void db_getAllPupils(db_t * self, pupils_t * arr){
    sqlite3_stmt * stmt = NULL;
    const char * sqlQuery = "SELECT * FROM PUPILS ";
    int count = 0;
    sqlite3_prepare_v2(self->db, sqlQuery, strlen(sqlQuery), &stmt, 0);
    while (1) {
        int rc = sqlite3_step(stmt);
        if (SQLITE_ERROR == rc) {
            printf("can't select pupils\n");
            exit(1);
        } else if (SQLITE_DONE == rc) {
            break;
        } else {
            _fillPupil(stmt, &arr[count]);
            count++;
        }
    }
    sqlite3_finalize(stmt);
}
