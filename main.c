#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

const __uint32_t ID_SIZE = size_of_attribute(Row, id);
const __uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const __uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const __uint32_t ID_OFFSET = 0;
const __uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const __uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const __uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;


void serialize_row(Row* source, void* dest){
  memcpy(dest+ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(dest+USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
  memcpy(dest+EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}


void deserialize_row(void* source, Row* dest){
  memcpy(&(dest->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(dest->username), source+USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(dest->email), source+EMAIL_OFFSET, EMAIL_SIZE);
}

const __uint32_t PAGE_SIZE = 4096;
const __uint32_t TABLE_MAX_PAGES = 100;
const __uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const __uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

struct Table_t {
  void* pages[TABLE_MAX_PAGES];
  __uint32_t num_rows;
};
typedef struct Table_t Table;

void* row_slot(Table* table, __uint32_t row_num){
  __uint32_t page_num = row_num / ROWS_PER_PAGE;
  void* page = table->pages[page_num];
  // alloc memory in case there is no page.
  if(!page) page = table->pages[page_num] = malloc(PAGE_SIZE);
  __uint32_t row_offset = row_num % ROWS_PER_PAGE;
  __uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

struct InputBuffer_t {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
};
typedef struct InputBuffer_t InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}


void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}


enum MetaCommandResult_t {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
};
typedef enum MetaCommandResult_t MetaCommandResult;

enum PrepareResult_t { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_COMMAND, PREPARE_SYNTAX_ERROR};
typedef enum PrepareResult_t PrepareResult;


MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

enum StatementType_t { STATEMENT_INSERT, STATEMENT_SELECT };
typedef enum StatementType_t StatementType;

const __uint32_t COLUMN_USERNAME_SIZE = 32;
const __uint32_t COLUMN_EMAIL_SIZE = 255;

struct Row_t {
  __uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
};

typedef struct Row_t Row;

struct Statement_t {
  StatementType type;
  Row row_to_insert;
};
typedef struct Statement_t Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){
  if(strncmp(input_buffer->buffer, "insert", 6) == 0){
    statement->type = STATEMENT_INSERT;
    int num_args = sscanf(input_buffer->buffer, "insert %d %s %s", 
      &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);
    if(num_args < 3) return PREPARE_SYNTAX_ERROR;
    return PREPARE_SUCCESS;
  }
  
  if(strncmp(input_buffer->buffer, "select", 6) == 0){
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_COMMAND;
}

void execute_statement(Statement* statement){
  switch(statement->type){
    case (STATEMENT_INSERT):
      printf("We do an insert here\n");
      break;
    case (STATEMENT_SELECT):
      printf("We do a select here\n");
      break;
  }
}

void print_prompt(){ printf("db > ");}

int main(int argc, char* argv[]) {
  InputBuffer* input_buffer = new_input_buffer();
  while (1) {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.') {
      switch(do_meta_command(input_buffer)){
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
         printf("Unrecognized command '%s'.\n", input_buffer->buffer);
         continue;
      }
    } 

    Statement statement;
    switch(prepare_statement(input_buffer, &statement)){
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_COMMAND):
        printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
        continue;
    }

    execute_statement(&statement);
  }
}



