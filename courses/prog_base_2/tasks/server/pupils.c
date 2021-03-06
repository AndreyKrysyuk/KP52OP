#include "pupils.h"

#define MAX_AMOUNT_PUPILS 20

typedef struct{
    char nameForm[WORD_LENGTH];
    int numberInList;
} form_t;

typedef struct pupil_s * pupil_t;

struct pupils_s{
    int amountPupils;
    pupil_t pupils[MAX_AMOUNT_PUPILS];
};

struct pupil_s{
    int id;
    char name[WORD_LENGTH];
    char surname[WORD_LENGTH];
    char birthdate[WORD_LENGTH];
    double score;
    form_t form;
};

pupils_t pupils_new(){
    pupils_t pupils = malloc(sizeof(struct pupils_s));
    pupils->amountPupils = 0;
    return pupils;
}

void pupils_remove(pupils_t self){
    for(int i = 0; i < self->amountPupils; i++){
        free(self->pupils[i]);
        self->pupils[i] = NULL;
    }
    free(self);
    self = NULL;
}

void pupils_print(pupils_t self){
    for(int i = 0; i < self->amountPupils; i++){
        printf("Pupil:\n"
               "\tID: %i\n"
               "\tName: %s\n"
               "\tSurname: %s\n"
               "\tBirthdate: %s\n"
               "\tScore: %.2f\n"
               "\tForm name: %s\n"
               "\tNumber pupil in list: %i\n",
               self->pupils[i]->id,
               self->pupils[i]->name,
               self->pupils[i]->surname,
               self->pupils[i]->birthdate,
               self->pupils[i]->score,
               self->pupils[i]->form.nameForm,
               self->pupils[i]->form.numberInList
               );
    }
}

int pupils_parseFromXML(pupils_t self, char* fileName){
	char line[1000];
    char text[10000] = "";
    FILE * fr = fopen(fileName, "r");
    while(fgets(line, 1000, fr)) {
        strcat(text, line);
    }
    xmlDoc * xDoc;
    xDoc = xmlReadMemory(text, strlen(text), NULL, NULL, 0);
    if (NULL == xDoc) {
        printf("Error parsing xml from memory");
        return 1;
    }
    xmlNode * xRootEl;
    xRootEl = xmlDocGetRootElement(xDoc);
    for(xmlNode * xCur = xRootEl->children; NULL != xCur ; xCur = xCur->next) {
        if (XML_ELEMENT_NODE == xCur->type) {
            self->pupils[self->amountPupils] = malloc(sizeof(struct pupil_s));
            for(xmlNode * xJ = xCur->children; NULL != xJ ; xJ = xJ->next) {
                if (XML_ELEMENT_NODE == xJ->type) {
                    if(strcmp(xJ->name, "form") == 0) {
                        xmlNode * xForm = xJ;
                        xmlNode * xNum = xForm->children->next;
                        char * nameForm = xmlGetProp(xForm, "nameForm");
                        strcpy(self->pupils[self->amountPupils]->form.nameForm, nameForm);
                        char * numberInlist = xmlNodeGetContent(xNum);
                        self->pupils[self->amountPupils]->form.numberInList = atoi(numberInlist);
                        continue;
                    } else if(strcmp(xJ->name, "name") == 0){
                        const char * name = xmlNodeGetContent(xJ);
                        strcpy(self->pupils[self->amountPupils]->name, name);
                    } else if(strcmp(xJ->name, "surname") == 0){
                        const char * surname = xmlNodeGetContent(xJ);
                        strcpy(self->pupils[self->amountPupils]->surname, surname);
                    } else if(strcmp(xJ->name, "birthdate") == 0){
                        const char * birthdate = xmlNodeGetContent(xJ);
                        strcpy(self->pupils[self->amountPupils]->birthdate, birthdate);
                    } else if(strcmp(xJ->name, "score") == 0){
                        const char * score = xmlNodeGetContent(xJ);
                        self->pupils[self->amountPupils]->score = atof(score);
                    } else if(strcmp(xJ->name, "id") == 0){
                        const char * id = xmlNodeGetContent(xJ);
                        self->pupils[self->amountPupils]->id = atoi(id);
                }
            }
        }
        self->amountPupils++;
    }
}
    fclose(fr);
    return 0;
}

char * pupils_allPupilsToMessage(pupils_t self){
    char message[MSG_LENGTH] = "\0";
    char buff[MSG_LENGTH];
    strcat(message, "<pupils>\n");
    for(int i = 0; i < self->amountPupils; i++){
        sprintf(buff, "\t<pupil>\n"
               "\t\t<id>%i</id>\n"
               "\t\t<name>%s</name>\n"
               "\t\t<surname>%s</surname>\n"
               "\t\t<birthdate>%s</birthdate>\n"
               "\t\t<form nameForm=\"%s\">\n"
               "\t\t\t<numberInList>%i</numberInList>\n"
               "\t\t</form>\n"
               "\t\t<score>%.2f</score>\n"
               "\t</pupil>\n",
               self->pupils[i]->id,
               self->pupils[i]->name,
               self->pupils[i]->surname,
               self->pupils[i]->birthdate,
               self->pupils[i]->form.nameForm,
               self->pupils[i]->form.numberInList,
               self->pupils[i]->score
               );
        strcat(message, buff);
    }
    strcat(message, "</pupils>\n");
    return message;
}

char * pupils_pupilByIdToMessage(pupils_t self, int id){
     char message[MSG_LENGTH];
     for(int i = 0; i < self->amountPupils; i++){
        if(self->pupils[i]->id == id){
            sprintf(message, "<pupil>\n"
                    "\t<id>%i</id>\n"
                    "\t<name>%s</name>\n"
                    "\t<surname>%s</surname>\n"
                    "\t<birthdate>%s</birthdate>\n"
                    "\t<form nameForm=\"%s\">\n"
                    "\t\t<numberInList>%i</numberInList>\n"
                    "\t</form>\n"
                    "\t<score>%.2f</score>\n"
                    "</pupil>\n\n",
                   self->pupils[i]->id,
                   self->pupils[i]->name,
                   self->pupils[i]->surname,
                   self->pupils[i]->birthdate,
                   self->pupils[i]->form.nameForm,
                   self->pupils[i]->form.numberInList,
                   self->pupils[i]->score
                   );
            return message;
        }
    }
    return 0;
}

void pupils_changePupil(pupils_t self, int id, int newId, const char * name, const char * surname, const char * birthdate,
                       double score, int numberInList, const char * nameForm){
        for(int i = 0; i < self->amountPupils; i++){
            if(self->pupils[i]->id == id){
               self->pupils[i]->id = newId;
               strcpy(self->pupils[i]->name, name);
               strcpy(self->pupils[i]->surname, surname);
               strcpy(self->pupils[i]->birthdate, birthdate);
               self->pupils[i]->score = score;
               strcpy(self->pupils[i]->form.nameForm, nameForm);
               self->pupils[i]->form.numberInList = numberInList;
            }
    }
}

void pupils_deletePupil(pupils_t self, int id){
    int index;
    for(int i = 0; i < self->amountPupils; i++){
            if(self->pupils[i]->id == id){
                    index = i;
            }
    }
        pupil_t tmp;
        for(int i = index; i < self->amountPupils; i++){
            tmp = self->pupils[i];
            self->pupils[i] = self->pupils[i + 1];
            self->pupils[i + 1] = tmp;
        }
        free(self->pupils[self->amountPupils]);
        self->amountPupils--;
}

int pupils_checkID(pupils_t self, int id){
    for(int i = 0; i < self->amountPupils; i++){
        if(self->pupils[i]->id == id){
            return 0;
        }
    }
    return 1;
}
