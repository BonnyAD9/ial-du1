/* ******************************* c204.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c204 - Převod infixového výrazu na postfixový (s využitím c202)     */
/*  Referenční implementace: Petr Přikryl, listopad 1994                      */
/*  Přepis do jazyka C: Lukáš Maršík, prosinec 2012                           */
/*  Upravil: Kamil Jeřábek, září 2019                                         */
/*           Daniel Dolejška, září 2021                                       */
/* ************************************************************************** */
/*
** Implementujte proceduru pro převod infixového zápisu matematického výrazu
** do postfixového tvaru. Pro převod využijte zásobník (Stack), který byl
** implementován v rámci příkladu c202. Bez správného vyřešení příkladu c202
** se o řešení tohoto příkladu nepokoušejte.
**
** Implementujte následující funkci:
**
**    infix2postfix ... konverzní funkce pro převod infixového výrazu
**                      na postfixový
**
** Pro lepší přehlednost kódu implementujte následující pomocné funkce:
**
**    untilLeftPar ... vyprázdnění zásobníku až po levou závorku
**    doOperation .... zpracování operátoru konvertovaného výrazu
**
** Své řešení účelně komentujte.
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

#include "c204.h"

bool solved;

/**
 * Pomocná funkce untilLeftPar.
 * Slouží k vyprázdnění zásobníku až po levou závorku, přičemž levá závorka bude
 * také odstraněna.
 * Pokud je zásobník prázdný, provádění funkce se ukončí.
 *
 * Operátory odstraňované ze zásobníku postupně vkládejte do výstupního pole
 * znaků postfixExpression.
 * Délka převedeného výrazu a též ukazatel na první volné místo, na které se má
 * zapisovat, představuje parametr postfixExpressionLength.
 *
 * Aby se minimalizoval počet přístupů ke struktuře zásobníku, můžete zde
 * nadeklarovat a používat pomocnou proměnnou typu char.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void untilLeftPar( Stack *stack, char *postfixExpression, unsigned *postfixExpressionLength ) {
	char c;
	// pop until stack is empty or there is '('
	while (!Stack_IsEmpty(stack)) {
		Stack_Top(stack, &c);
		Stack_Pop(stack);

		// don't add '(' to the result
		if (c == '(') {
			return;
		}

		postfixExpression[(*postfixExpressionLength)++] = c;
	}
}

/// Returns the precedence of the oprator or 0 if it is unknown.
/// (higher precedence - evaluates first)
int getPrecedence(char c) {
	switch (c) {
	case '+':
	case '-':
		return 2;
	case '*':
	case '/':
		return 3;
	}
	return 0;
}

/**
 * Pomocná funkce doOperation.
 * Zpracuje operátor, který je předán parametrem c po načtení znaku ze
 * vstupního pole znaků.
 *
 * Dle priority předaného operátoru a případně priority operátoru na vrcholu
 * zásobníku rozhodneme o dalším postupu.
 * Délka převedeného výrazu a taktéž ukazatel na první volné místo, do kterého
 * se má zapisovat, představuje parametr postfixExpressionLength, výstupním
 * polem znaků je opět postfixExpression.
 *
 * @param stack Ukazatel na inicializovanou strukturu zásobníku
 * @param c Znak operátoru ve výrazu
 * @param postfixExpression Znakový řetězec obsahující výsledný postfixový výraz
 * @param postfixExpressionLength Ukazatel na aktuální délku výsledného postfixového výrazu
 */
void doOperation( Stack *stack, char c, char *postfixExpression, unsigned *postfixExpressionLength ) {
	char top = 0;
	if (!Stack_IsEmpty(stack)) {
		Stack_Top(stack, &top);
	}

	// In case that the stack is empty or the top value is '(', getPrecedence
	// will return 0 and this condition will not pass, because `c` is always
	// operator so the precedence is > 0
	if (getPrecedence(c) <= getPrecedence(top)) {
		// Add the top operator from the stack only when it has higher precedence
		Stack_Pop(stack);
		postfixExpression[(*postfixExpressionLength)++] = top;

		// for special case such as a+b*c*d-e, where after pop the next
		// operator on top should be inserted before this one
		doOperation(stack, c, postfixExpression, postfixExpressionLength);
		return;
	}

	// Always push the current operator
	Stack_Push(stack, c);
}

/**
 * Konverzní funkce infix2postfix.
 * Čte infixový výraz ze vstupního řetězce infixExpression a generuje
 * odpovídající postfixový výraz do výstupního řetězce (postup převodu hledejte
 * v přednáškách nebo ve studijní opoře).
 * Paměť pro výstupní řetězec (o velikosti MAX_LEN) je třeba alokovat. Volající
 * funkce, tedy příjemce konvertovaného řetězce, zajistí korektní uvolnění zde
 * alokované paměti.
 *
 * Tvar výrazu:
 * 1. Výraz obsahuje operátory + - * / ve významu sčítání, odčítání,
 *    násobení a dělení. Sčítání má stejnou prioritu jako odčítání,
 *    násobení má stejnou prioritu jako dělení. Priorita násobení je
 *    větší než priorita sčítání. Všechny operátory jsou binární
 *    (neuvažujte unární mínus).
 *
 * 2. Hodnoty ve výrazu jsou reprezentovány jednoznakovými identifikátory
 *    a číslicemi - 0..9, a..z, A..Z (velikost písmen se rozlišuje).
 *
 * 3. Ve výrazu může být použit předem neurčený počet dvojic kulatých
 *    závorek. Uvažujte, že vstupní výraz je zapsán správně (neošetřujte
 *    chybné zadání výrazu).
 *
 * 4. Každý korektně zapsaný výraz (infixový i postfixový) musí být uzavřen
 *    ukončovacím znakem '='.
 *
 * 5. Při stejné prioritě operátorů se výraz vyhodnocuje zleva doprava.
 *
 * Poznámky k implementaci
 * -----------------------
 * Jako zásobník použijte zásobník znaků Stack implementovaný v příkladu c202.
 * Pro práci se zásobníkem pak používejte výhradně operace z jeho rozhraní.
 *
 * Při implementaci využijte pomocné funkce untilLeftPar a doOperation.
 *
 * Řetězcem (infixového a postfixového výrazu) je zde myšleno pole znaků typu
 * char, jenž je korektně ukončeno nulovým znakem dle zvyklostí jazyka C.
 *
 * Na vstupu očekávejte pouze korektně zapsané a ukončené výrazy. Jejich délka
 * nepřesáhne MAX_LEN-1 (MAX_LEN i s nulovým znakem) a tedy i výsledný výraz
 * by se měl vejít do alokovaného pole. Po alokaci dynamické paměti si vždycky
 * ověřte, že se alokace skutečně zdrařila. V případě chyby alokace vraťte namísto
 * řetězce konstantu NULL.
 *
 * @param infixExpression vstupní znakový řetězec obsahující infixový výraz k převedení
 *
 * @returns znakový řetězec obsahující výsledný postfixový výraz
 */
char *infix2postfix( const char *infixExpression ) {
	char *res = malloc(sizeof(char) * MAX_LEN);
	if (!res) {
		return NULL;
	}
	unsigned i = 0;

	// The stack contains only operators and '('
	Stack stack;
	error_flag = false;
	Stack_Init(&stack);
	if (error_flag) {
		return NULL;
	}

	// Cur points to the current char. At the start there is no current
	// char, so it points to the one before (the first value doesn't have to
	// point to valid location because it is never dereferenced)
	const char *cur = infixExpression - 1;

	while (*++cur && *cur != '=') {
		switch (*cur)
		{
		case '(':
			Stack_Push(&stack, *cur);
			break;
		case ')':
			untilLeftPar(&stack, res, &i);
			break;
		case '+':
		case '-':
		case '*':
		case '/':
			doOperation(&stack, *cur, res, &i);
			break;
		default:
			res[i++] = *cur;
			break;
		}
	}

	// Finish up
	untilLeftPar(&stack, res, &i);
	res[i++] = '=';
	res[i++] = 0;

	Stack_Dispose(&stack);

	return res;
}


/**
 * Pomocná metoda pro vložení celočíselné hodnoty na zásobník.
 *
 * Použitá implementace zásobníku aktuálně umožňuje vkládání pouze
 * hodnot o velikosti jednoho byte (char). Využijte této metody
 * k rozdělení a postupné vložení celočíselné (čtyřbytové) hodnoty
 * na vrchol poskytnutého zásobníku.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value hodnota k vložení na zásobník
 */
void expr_value_push( Stack *stack, int value ) {
	char *arr = (char *)&value;
	for (size_t i = 0; i < sizeof(value); ++i) {
		Stack_Push(stack, arr[i]);
	}
}

/**
 * Pomocná metoda pro extrakci celočíselné hodnoty ze zásobníku.
 *
 * Využijte této metody k opětovnému načtení a složení celočíselné
 * hodnoty z aktuálního vrcholu poskytnutého zásobníku. Implementujte
 * tedy algoritmus opačný k algoritmu použitému v metodě
 * `expr_value_push`.
 *
 * @param stack ukazatel na inicializovanou strukturu zásobníku
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné celočíselné hodnoty z vrcholu zásobníku
 */
void expr_value_pop( Stack *stack, int *value ) {
	char *arr = (char *)value;
	for (size_t i = sizeof(*value); i > 0; --i) {
		Stack_Top(stack, arr + i - 1);
		Stack_Pop(stack);
	}
}

/// Gets the value of the given variable. Finds it in the array.
/// When there is no variable with that name, returns 0.
int getVariable(VariableValue *values, size_t valueLen, char name) {
	// Sequentialy search in the array
	for (; valueLen--; ++values) {
		if (values->name == name) {
			return values->value;
		}
	}
	return 0;
}

/// Performs the operation of the given operator on the two values on the top
/// of the stack (pops the values), and pushes the result to the stack.
bool calculate(Stack *stack, char operator) {
	int a, b;
	// The values are in reverse on the stack
	expr_value_pop(stack, &b);
	expr_value_pop(stack, &a);

	int res;
	switch (operator)
	{
	case '+':
		res = a + b;
		break;
	case '-':
		res = a - b;
		break;
	case '*':
		res = a * b;
		break;
	case '/':
		if (b == 0) {
			return false;
		}
		res = a / b;
		break;
	default:
		res = 0;
		break;
	}

	expr_value_push(stack, res);
	return true;
}

/// Can't use the header ctype so I have my own implementation
static int is_digit(int value) {
	return value >= '0' && value <= '9';
}

#define isdigit(value) is_digit(value)

/**
 * Tato metoda provede vyhodnocení výrazu zadaném v `infixExpression`,
 * kde hodnoty proměnných použitých v daném výrazu jsou definovány
 * v poli `variableValues`.
 *
 * K vyhodnocení vstupního výrazu využijte implementaci zásobníku
 * ze cvičení c202. Dále také využijte pomocných funkcí `expr_value_push`,
 * respektive `expr_value_pop`. Při řešení si můžete definovat libovolné
 * množství vlastních pomocných funkcí.
 *
 * Předpokládejte, že hodnoty budou vždy definovány
 * pro všechy proměnné použité ve vstupním výrazu.
 *
 * @param infixExpression vstpní infixový výraz s proměnnými
 * @param variableValues hodnoty proměnných ze vstupního výrazu
 * @param variableValueCount počet hodnot (unikátních proměnných
 *   ve vstupním výrazu)
 * @param value ukazatel na celočíselnou proměnnou pro uložení
 *   výsledné hodnoty vyhodnocení vstupního výrazu
 *
 * @return výsledek vyhodnocení daného výrazu na základě poskytnutých hodnot proměnných
 */
bool eval( const char *infixExpression, VariableValue variableValues[], int variableValueCount, int *value ) {
	char *pfix = infix2postfix(infixExpression);
	if (!pfix) {
		return false;
	}

	// The stack contains int values, at the end of the function, it should
	// contain only one value.
	Stack stack;
	error_flag = false;
	Stack_Init(&stack);
	if (error_flag) {
		return false;
	}

	// arr points to the current char, at the start there is no current char
	// so it points to the value before (the first value doesn't have to point
	// to valid location because it is never dereferenced)
	char *arr = pfix - 1;
	while (*++arr && *arr != '=') {
		// push all literal values
		if (isdigit(*arr)) {
			expr_value_push(&stack, *arr - '0');
			continue;
		}

		switch (*arr) {
		case '+':
		case '-':
		case '*':
		case '/':
			if (!calculate(&stack, *arr)) {
				free(pfix);
				Stack_Dispose(&stack);
				return false;
			}
			continue;
		default:
			expr_value_push(&stack, getVariable(variableValues, variableValueCount, *arr));
			continue;
		}
	}

	if (*arr != '=') {
		return false;
	}

	// the only remaining value on the stack is the result
	expr_value_pop(&stack, value);

	// finish up
	free(pfix);
	Stack_Dispose(&stack);

	return true;
}

/* Konec c204.c */
