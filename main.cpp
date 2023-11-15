#include <iostream>
#include <array>

enum AvailableTypes {TEXT, NUMBER, NONE};
enum Operations {SUM, PRODUCT, AVERAGE, EMPTY};
enum CellType {DATA, FORMULAWITHRESULT, EMPTYRESULT, VOID};
const size_t MAXSIZE = 100;

class TableCell{
public:
    TableCell();
    explicit TableCell(std::string& value);
    explicit TableCell(double value);
    TableCell(const TableCell& copy);
    ~TableCell() = default;
    virtual void print() const;
    [[nodiscard]] double getNumberData() const;
    [[nodiscard]] std::string getTextData() const;
    [[nodiscard]] char getDataType() const;
    void update(std::string& new_value);
    void update(double new_value);
    [[nodiscard]] virtual std::string cellType() const;
protected:
    std::string dataText;
    double dataNumber;
    AvailableTypes type;
};

class CellWithFormula: public TableCell {
public:
    CellWithFormula();
    CellWithFormula(const std::pair<int, int>& start_cell, const std::pair<int,
            int>& end_cell, char type);
    CellWithFormula(const CellWithFormula& copy);
    explicit CellWithFormula(const TableCell& copy);
    void print() const override;
    [[nodiscard]] std::pair<int, int> getStartCell() const;
    [[nodiscard]] std::pair<int, int> getEndCell() const;
    [[nodiscard]] char getOperationType() const;
    void update(const std::pair<int, int>& start_cell, const std::pair<int,
            int>& end_cell, char type);
    [[nodiscard]] std::string cellType() const override;
    double result{};
private:
    std::pair<int, int> start;
    std::pair<int, int> end;
    Operations operation;
};

class Table: public CellWithFormula {
public:
    Table();
    Table(int row, int column, const TableCell& cell);
    Table(int row, int column, const CellWithFormula& cell);
    Table(const Table& copy);
    void print(int row, int column) const;
    void update(int row, int column, const TableCell& cell);
    void update(int row, int column, const CellWithFormula& cell);
    [[nodiscard]] double getCellData(int row, int column) const;
    [[nodiscard]] char getCellType(int row, int column) const;
    [[nodiscard]] std::string cellType() const override;
private:
    void check();
    std::array<std::array<CellWithFormula, MAXSIZE>, MAXSIZE> data{};
    std::array<std::array<CellType, MAXSIZE>, MAXSIZE> type{};
};

TableCell::TableCell() {
    dataText = "";
    dataNumber = 0.0;
    type = NONE;
}

TableCell::TableCell(std::string& value) {
    dataText = value;
    dataNumber = 0.;
    type = TEXT;
}

TableCell::TableCell(double value) {
    dataText = "";
    dataNumber = value;
    type = NUMBER;
}

TableCell::TableCell(const TableCell& copy) {
    dataText = copy.dataText;
    dataNumber = copy.dataNumber;
    type = copy.type;
}

void TableCell::print() const {
    if (type == TEXT) {
        std::cout << "Data: " << dataText << '\n';
        return;
    }
    if (type == NUMBER) {
        std::cout << "Data: " << dataNumber << '\n';
    }
}

double TableCell::getNumberData() const {
    return dataNumber;
}

std::string TableCell::getTextData() const {
    return dataText;
}

char TableCell::getDataType() const {
    switch (type) {
        case NUMBER: return '0';
        case TEXT: return 's';
        case NONE: return 'E';
    }
}

void TableCell::update(std::string& new_value) {
    dataText = new_value;
    dataNumber = 0;
    type = TEXT;
}

void TableCell::update(double new_value) {
    dataText = "";
    dataNumber = new_value;
    type = NUMBER;
}

std::string TableCell::cellType() const {
    std::string typeOfCell = "Data cell";
    return typeOfCell;
}

CellWithFormula::CellWithFormula() : TableCell() {
    std::pair<int, int> temp{0, 0};
    start = temp;
    end = temp;
    result = 0.;
    operation = EMPTY;
}

CellWithFormula::CellWithFormula(const std::pair<int, int>& start_cell,
                                 const std::pair<int, int>& end_cell,
                                 char type): TableCell() {
    start = start_cell;
    end = end_cell;
    result = 0.;
    switch (type) {
        case '+': operation = SUM;
            break;
        case '*': operation = PRODUCT;
            break;
        case '~': operation = AVERAGE;
            break;
        default: operation = EMPTY;
    }
}

CellWithFormula::CellWithFormula(const CellWithFormula &copy)  :
TableCell(copy) {
    start = copy.start;
    end = copy.end;
    result = copy.result;
    operation = copy.operation;
}

CellWithFormula::CellWithFormula(const TableCell& copy) {
    operation = EMPTY;
    dataText = copy.getTextData();
    dataNumber = copy.getNumberData();
    switch (copy.getDataType()) {
        case '0': type = NUMBER;
            break;
        case 's': type = TEXT;
            break;
        case 'E': type = NONE;
    }
}

void CellWithFormula::print() const {
    if (operation == EMPTY) {
        std::cout << "No formula is found\n";
        return;
    }
    std::cout << "Result: " << result << '\n';
}

std::pair<int, int> CellWithFormula::getStartCell() const {
    return start;
}

std::pair<int, int> CellWithFormula::getEndCell() const {
    return end;
}

char CellWithFormula::getOperationType() const {
    switch (operation) {
        case SUM: return '+';
        case PRODUCT: return '*';
        case AVERAGE: return '~';
        case EMPTY: return '?';
    }
}

void CellWithFormula::update(const std::pair<int, int>& start_cell,
                             const std::pair<int, int>& end_cell, char type) {
    start = start_cell;
    end = end_cell;
    switch (type) {
        case '+': operation = SUM;
            break;
        case '*': operation = PRODUCT;
            break;
        case '~': operation = AVERAGE;
            break;
        default: operation = EMPTY;
    }
}

std::string CellWithFormula::cellType() const {
    std::string typeOfCell = "Formula cell";
    return typeOfCell;
}

Table::Table(): CellWithFormula() {
    std::array<CellType, MAXSIZE> temp{};
    temp.fill(VOID);
    type.fill(temp);
}

Table::Table(int row, int column, const TableCell &cell):
CellWithFormula(cell) {
    std::array<CellType, MAXSIZE> temp{};
    temp.fill(VOID);
    type.fill(temp);
    data[row][column] = CellWithFormula(cell);
    type[row][column] = DATA;
}

Table::Table(int row, int column, const CellWithFormula &cell) :
CellWithFormula(cell) {
    std::array<CellType, MAXSIZE> temp{};
    temp.fill(VOID);
    type.fill(temp);
    data[row][column] = cell;
    type[row][column] = EMPTYRESULT;
}

Table::Table(const Table &copy): CellWithFormula(copy) {
    data = copy.data;
    type = copy.type;
}

void Table::print(int row, int column) const {
    if (type[row][column] == DATA) {
        switch (data[row][column].getDataType()) {
            case '0': std::cout << data[row][column].getNumberData() << '\n';
                break;
            case 's': std::cout << data[row][column].getTextData() << '\n';
                break;
            case 'E': std::cout << "Cell is empty\n";
        }
    }
    else {
        if (type[row][column] == FORMULAWITHRESULT) {
            switch (data[row][column].getOperationType()) {
                case '?': std::cout << "Cell is empty\n";
                    break;
                default: std::cout << data[row][column].result << '\n';
            }
        }
        else
            std::cout << "Cell is empty\n";
    }
}

void Table::check() {
    for (size_t i = 0; i < MAXSIZE; ++i)
        for (size_t j = 0; j < MAXSIZE; ++j)
            if (type[i][j] == EMPTYRESULT || type[i][j] == FORMULAWITHRESULT) {
                int l_1 = data[i][j].getStartCell().first,
                r_1 = data[i][j].getStartCell().second;
                int l_2 = data[i][j].getEndCell().first,
                r_2 = data[i][j].getEndCell().second;
                switch (data[i][j].getOperationType()) {
                    case '+': {
                        double curr_sum = 0;
                        bool f = true;
                        for (int x = l_1; x <= l_2 && f; ++x)
                            for (int y = r_1; y <= r_2 && f; ++y) {
                                if (type[x][y] != DATA ||
                                (type[x][y] == DATA &&
                                data[x][y].getDataType() == 's'))
                                    f = false;
                                else
                                    curr_sum += data[x][y].getNumberData();
                            }
                        if (f) {
                            data[i][j].result = curr_sum;
                            type[i][j] = FORMULAWITHRESULT;
                        }
                        else
                            type[i][j] = EMPTYRESULT;
                    }
                        break;
                    case '*': {
                        double curr_prod = 1;
                        bool f = true;
                        for (int x = l_1; x <= l_2 && f; ++x)
                            for (int y = r_1; y <= r_2 && f; ++y)
                                if (type[x][y] != DATA ||
                                (type[x][y] == DATA &&
                                data[x][y].getDataType() == 's'))
                                    f = false;
                                else
                                    curr_prod *= data[x][y].getNumberData();
                        if (f) {
                            data[i][j].result = curr_prod;
                            type[i][j] = FORMULAWITHRESULT;
                        }
                        else
                            type[i][j] = EMPTYRESULT;
                    }
                        break;
                    case '~': {
                        double curr_sum = 0;
                        bool f = true;
                        for (int x = l_1; x <= l_2 && f; ++x)
                            for (int y = r_1; y <= r_2 && f; ++y)
                                if (type[x][y] != DATA ||
                                (type[x][y] == DATA &&
                                data[x][y].getDataType() == 's'))
                                    f = false;
                                else
                                    curr_sum += data[x][y].getNumberData();
                        if (f){
                            data[i][j].result = curr_sum /
                                    ((l_2 - l_1 + 1) * (r_2 - r_1 + 1));
                            type[i][j] = FORMULAWITHRESULT;
                        }
                        else
                            type[i][j] = EMPTYRESULT;
                    }
                }
            }
}

void Table::update(int row, int column, const TableCell &cell) {
    data[row][column] = CellWithFormula(cell);
    type[row][column] = DATA;
    check();
}

void Table::update(int row, int column, const CellWithFormula &cell) {
    data[row][column] = cell;
    type[row][column] = EMPTYRESULT;
    check();
}

double Table::getCellData(int row, int column) const {
    if (type[row][column] == DATA && data[row][column].getDataType() == '0')
        return data[row][column].getNumberData();
    else
        return data[row][column].result;
}

char Table::getCellType(int row, int column) const {
    if (type[row][column] == DATA)
        return 'd';
    if (type[row][column] == FORMULAWITHRESULT)
        return '+';
    if (type[row][column] == EMPTYRESULT)
        return '-';
    return '?';
}

std::string Table::cellType() const  {
    std::string typeofCell = "Table of cells";
    return typeofCell;
}

int main() {
    TableCell ex_1;
    assert(ex_1.getTextData().empty() && ex_1.getNumberData() == 0. &&
           ex_1.getDataType() == 'E');
    std::string text_ex = "example";
    TableCell ex_2(text_ex);
    assert(ex_2.getTextData() == "example" && ex_2.getNumberData() == 0. &&
           ex_2.getDataType() == 's');
    TableCell ex_3(1.);
    assert(ex_3.getTextData().empty() && ex_3.getNumberData() == 1. &&
           ex_3.getDataType() == '0');
    TableCell ex_4(ex_3);
    assert(ex_4.getTextData() == ex_3.getTextData() &&
           ex_4.getNumberData() == ex_3.getNumberData() &&
           ex_4.getDataType() == ex_3.getDataType());
    ex_2.print();
    ex_3.print();
    ex_2.update(5.);
    assert(ex_2.getTextData().empty() && ex_2.getNumberData() == 5. &&
           ex_2.getDataType() == '0');
    ex_3.update(text_ex);
    assert(ex_3.getTextData() == "example" && !ex_3.getNumberData() &&
           ex_3.getDataType() == 's');
    CellWithFormula t_1;
    assert(t_1.getTextData().empty() && t_1.getNumberData() == 0. &&
           t_1.getDataType() == 'E' && !t_1.getStartCell().first &&
           !t_1.getStartCell().second && !t_1.getEndCell().first &&
           !t_1.getEndCell().second && t_1.result == 0. &&
           t_1.getOperationType() == '?');
    std::pair<int, int> t_2_start{1, 1};
    std::pair<int, int> t_2_end{5, 5};
    CellWithFormula t_2(t_2_start, t_2_end, '+');
    assert(t_2.getTextData().empty() && t_2.getNumberData() == 0. &&
           t_2.getDataType() == 'E' && t_2.getStartCell().first == 1 &&
           t_2.getStartCell().second == 1 && t_2.getEndCell().first == 5 &&
           t_2.getEndCell().second == 5 && t_2.getOperationType() == '+' &&
           t_2.result == 0.);
    CellWithFormula t_3(t_2);
    assert(t_3.getNumberData() == t_2.getNumberData() &&
           t_3.getTextData() == t_2.getTextData() &&
           t_3.getDataType() == t_2.getDataType() &&
           t_3.getStartCell() == t_2.getStartCell() &&
           t_3.getEndCell() == t_2.getEndCell() &&
           t_3.getOperationType() == t_2.getOperationType() &&
           t_3.result == t_2.result);
    t_1.print();
    t_2.print();
    std::pair<int, int> t_3_start{2, 2};
    std::pair<int, int> t_3_end{6, 6};
    t_3.update(t_3_start, t_3_end, '*');
    assert(t_3.getStartCell().first == 2 && t_3.getStartCell().second == 2 &&
           t_3.getEndCell().first == 6 && t_3.getEndCell().second == 6 &&
           t_3.getOperationType() == '*');
    assert(ex_1.cellType() == "Data cell" && t_1.cellType() == "Formula cell");
    Table tab_1;
    assert(tab_1.getNumberData() == 0. && tab_1.getTextData().empty() &&
           tab_1.getDataType() == 'E' && !tab_1.getStartCell().first &&
           !tab_1.getStartCell().second && !tab_1.getEndCell().first &&
           !tab_1.getEndCell().second && tab_1.getOperationType() == '?' &&
           tab_1.getCellData(0, 0) == 0. && tab_1.getCellType(0, 0) == '?');
    Table tab_2(0, 0, ex_2);
    assert(tab_2.getNumberData() == 5. && tab_2.getTextData().empty() &&
           tab_2.getDataType() == '0' && !tab_2.getStartCell().first &&
           !tab_2.getStartCell().second && !tab_2.getEndCell().first &&
           !tab_2.getEndCell().second && tab_2.getOperationType() == '?' &&
           tab_2.getCellData(0, 0) == 5. && tab_2.getCellType(0, 0) == 'd');
    Table tab_3(0, 0, t_2);
    assert(tab_3.getNumberData() == 0. && tab_3.getTextData().empty() &&
           tab_3.getDataType() == 'E' && tab_3.getStartCell().first == 1 &&
           tab_3.getStartCell().second == 1 && tab_3.getEndCell().first == 5 &&
           tab_3.getEndCell().second == 5 && tab_3.getOperationType() == '+' &&
           tab_3.getCellData(0, 0) == 0. && tab_3.getCellType(0, 0) == '-');
    Table tab_4(tab_3);
    assert(tab_3.getNumberData() == tab_4.getNumberData() &&
           tab_3.getTextData() == tab_4.getTextData() &&
           tab_3.getDataType() == tab_4.getDataType() &&
           tab_3.getStartCell() == tab_4.getStartCell() &&
           tab_3.getEndCell() == tab_4.getEndCell() &&
           tab_3.getOperationType() == tab_4.getOperationType() &&
           tab_3.getCellData(0, 0) == tab_4.getCellData(0, 0) &&
           tab_3.getCellType(0, 0) == tab_4.getCellType(0, 0));
    tab_4.update(0, 0, ex_2);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(0, 0) == 5. && tab_4.getCellType(0, 0) == 'd');
    TableCell cell_2(2.);
    tab_4.update(0, 1, cell_2);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(0, 1) == 2. && tab_4.getCellType(0, 1) == 'd');
    TableCell cell_3(-4.5);
    tab_4.update(1, 0, cell_3);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(1, 0) == -4.5 && tab_4.getCellType(1, 0) == 'd');
    TableCell cell_4(0.23);
    tab_4.update(1, 1, cell_4);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(1, 1) == 0.23 && tab_4.getCellType(1, 1) == 'd');
    std::pair<int, int> start_1{0, 0};
    std::pair<int, int> end_1{1, 1};
    CellWithFormula f_1(start_1, end_1, '+');
    tab_4.update(2, 0, f_1);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(2, 0) == 2.73 && tab_4.getCellType(2, 0) == '+');
    std::pair<int, int> start_2{0, 0};
    std::pair<int, int> end_2{1, 1};
    CellWithFormula f_2(start_2, end_2, '*');
    tab_4.update(2, 1, f_2);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(2, 1) == -10.35 &&
           tab_4.getCellType(2, 1) == '+');
    std::pair<int, int> start_3{0, 0};
    std::pair<int, int> end_3{1, 1};
    CellWithFormula f_3(start_3, end_3, '~');
    tab_4.update(2, 2, f_3);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(2, 2) == 0.6825 &&
           tab_4.getCellType(2, 2) == '+');
    tab_4.print(0, 0);
    tab_4.print(0, 1);
    tab_4.print(1, 0);
    tab_4.print(1, 1);
    tab_4.print(2, 0);
    tab_4.print(2, 1);
    tab_4.print(2, 2);
    tab_4.update(1, 0, ex_3);
    assert(tab_4.getNumberData() == 0. && tab_4.getTextData().empty() &&
           tab_4.getDataType() == 'E' && tab_4.getStartCell().first == 1 &&
           tab_4.getStartCell().second == 1 && tab_4.getEndCell().first == 5 &&
           tab_4.getEndCell().second == 5 && tab_4.getOperationType() == '+' &&
           tab_4.getCellData(1, 0) == 0. && tab_4.getCellType(1, 0) == 'd');
    tab_4.print(0, 0);
    tab_4.print(0, 1);
    tab_4.print(1, 0);
    tab_4.print(1, 1);
    tab_4.print(2, 0);
    tab_4.print(2, 1);
    tab_4.print(2, 2);
    assert(tab_4.cellType() == "Table of cells");
    std::cout << "All tests have successfully passed";
    return 0;
}
