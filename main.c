#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

enum PrepareResult_t { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_COMMAND};
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

struct Statement_t {
  StatementType type;
};
typedef struct Statement_t Statement;

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement){
  if(strncmp(input_buffer->buffer, "insert", 6) == 0){
    statement->type = STATEMENT_INSERT;
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



