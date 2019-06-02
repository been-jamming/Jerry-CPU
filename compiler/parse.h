typedef enum token_type token_type;

enum token_type{
	TYPE,
	LITERAL,
	IDENTIFIER,
	CONTROL,
	OPERATOR,
	KEYWORD,
	UNKNOWN
};

typedef enum data_type data_type;

enum data_type{
	VOID,
	INT,
	CHAR
};

typedef enum sign_data sign_data;

enum sign_data{
	UNSIGNED,
	SIGNED,
	NONE
};

typedef enum literal_type literal_type;

enum literal_type{
	INTEGER,
	CHARACTER,
	STRING
};

typedef enum keyword keyword;

enum keyword{
	IF,
	WHILE,
	ELSE,
	FOR
};

typedef struct token token;

struct token{
	token_type type;
	union{
		//SIGNEDNESS
		sign_data sign;
		//TYPE
		data_type dtype;
		//LITERAL
		struct{
			literal_type ltype;
			union{
				int int_value;
				char char_value;
				char *string_value;
			};
		};
		//IDENTIFIER
		char *identifier;
		//CONTROL
		char control_character;
		//OPERATOR
		char operator_character;
		//KEYWORD
		keyword keyword_type;
		//UNKNOWN
		char unknown_character;
	};
};

