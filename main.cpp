#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
using namespace std;

class Cat;
class Dog;
class Parrot;

class Animals {
public:
    Animals() : age(0) {}

    Animals(const string& name1, const string& type, const string& name2, int age = 0)
            : name1(name1), type(type), name2(name2), age(age) {}

    string name1;
    string type;
    string name2;
    int age;

    string getName1() const {
        return name1;
    }
    string getType() const {
        return type;
    }

    string getName2() const {
        return name2;
    }

    int getAge() const {
        return age;
    }

    void setName1(const string& name) {
        name1 = name;
    }

    void setType(const string& animalType) {
        type = animalType;
    }

    void setName2(const string& animalName) {
        name2 = animalName;
    }

    void setAge(int animalAge) {
        age = animalAge;
    }

    void Print() {
        cout << "Владелец: " << name1 << " ";
        cout << "Тип животного: " << type << " ";
        if (!name2.empty()) {
            cout << "Имя животного: " << name2 << endl;
        }
        cout << "Возраст: " << age << " ";
    }

    virtual void Sound() = 0;
};


namespace AnimalFactory {

    class AnimalCreator {
    public:
        virtual unique_ptr<Animals> CreateAnimal(const string& name1, const string& name2, int age) const = 0;
        virtual ~AnimalCreator() {}
    };


    class CatFactory : public AnimalCreator {
    public:
        unique_ptr<Animals> CreateAnimal(const string& name1, const string& name2, int age) const override;
    };

    class DogFactory : public AnimalCreator {
    public:
        unique_ptr<Animals> CreateAnimal(const string& name1, const string& name2, int age) const override;
    };


    class ParrotFactory : public AnimalCreator {
    public:
        unique_ptr<Animals> CreateAnimal(const string& name1, const string& name2, int age) const override;
    };
}


class Cat : public Animals {
public:
    Cat(const string& name1, const string& name2, const int age) : Animals(name1, "Кот", name2, age) {}
    void Sound() override {
        cout << "Meow";
    }
};

class Dog : public Animals {
public:
    Dog(const string& name1, const string& name2, const int age) : Animals(name1, "Собака", name2, age) {}
    void Sound() override {
        cout << "Woof";
    }
};

class Parrot : public Animals {
public:
    Parrot(const string& name1, const string& name2, const int age) : Animals(name1, "Попугай", name2, age) {}
    void Sound() override {
        cout << "Quik-quik";
    }
};


unique_ptr<Animals> AnimalFactory::CatFactory::CreateAnimal(const string& name1, const string& name2, int age) const {
    return make_unique<Cat>(name1, name2, age);
}

unique_ptr<Animals> AnimalFactory::DogFactory::CreateAnimal(const string& name1, const string& name2, int age) const {
    return make_unique<Dog>(name1, name2, age);
}

unique_ptr<Animals> AnimalFactory::ParrotFactory::CreateAnimal(const string& name1, const string& name2, int age) const {
    return make_unique<Parrot>(name1, name2, age);
}

int main() {
    ifstream fin("animals.txt");
    if (!fin.is_open()) {
        cout << "Enable to open file";
    } else {
        string st;
        unordered_map<string, vector<unique_ptr<Animals>>> ownerAnimals;
        while (getline(fin, st)) {
            string name1, type, name2;
            int age;
            istringstream ss(st); // Создание потока
            getline(ss, name1, ',');
            getline(ss, type, ',');
            getline(ss, name2, ',');
            ss >> age;

            if (name2 == "") {
                name2 = "Noname";
            }
            unique_ptr<Animals> animal = nullptr; // Умный указатель
            if (type == "Кот") {
                animal = AnimalFactory::CatFactory().CreateAnimal(name1, name2, age);
            } else if (type == "Собака") {
                animal = AnimalFactory::DogFactory().CreateAnimal(name1, name2, age);
            } else if (type == "Попугай") {
                animal = AnimalFactory::ParrotFactory().CreateAnimal(name1, name2, age);
            }
            if (animal != nullptr) {
                ownerAnimals[name1].push_back(move(animal));
            } else {
                cerr << "Ошибка: Неизвестный тип животного: " << type << endl;
            }
        }
        fin.close();

        while (true) {
            cout << "МЕНЮ С ЗАДАНИЯМИ" << endl
                 << "1) Посчитать количество различных видов животных у каждого владельца" << endl
                 << "2) Вывести владельцев и клички для конкретного вида животного" << endl
                 << "3) Определить, сколько видов животных носит определённую кличку" << endl
                 << "4) Вывести информацию о возрасте самого старого и самого молодого животного каждого вида" << endl
                 << "5) Выйти из программы" << endl
                 << "Введите номер выбранного пункта: ";
            int k;
            cin >> k;

            if (cin.fail()) {
                cout << "Ошибка: Введите только число " << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }

            if (k == 5) {
                break;
            }
            switch (k) {
                case 1: {
                    for (auto& owner : ownerAnimals) {
                        cout << "Владелец: " << owner.first << ", Количество видов животных: " << owner.second.size() << endl;
                    }
                    break;
                }
                case 2: {
                    string animalType;
                    cout << "Введите вид животного: ";
                    cin >> animalType;
                    for (const auto& owner : ownerAnimals) {
                        for (const auto& animal : owner.second) {
                            if (animal->type == animalType) {
                                cout << "Владелец: " << owner.first << ", Кличка: " << (*animal).name2 << endl;
                                break;
                            }
                        }
                    }
                    break;
                }
                case 3: {
                    string petName;
                    cout << "Введите кличку: ";
                    cin >> petName;
                    unordered_set<string> animalTypes;
                    for (auto& owner : ownerAnimals) {
                        for (auto& animal : owner.second) {
                            if (animal->name2 == petName) {
                                animalTypes.insert(animal->type);
                            }
                        }
                    }
                    cout << "Количество видов животных с кличкой \"" << petName << "\": " << animalTypes.size() << endl;
                    break;
                }
                case 4: {
                    for (const auto& owner : ownerAnimals) {
                        unordered_map<string, pair<int, int>> minMaxAge;
                        for (const auto& animal : owner.second) {
                            auto& minMax = minMaxAge[animal->type];
                            if (minMax.first == 0 || animal->age < minMax.first) {
                                minMax.first = animal->age;
                            }
                            if (animal->age > minMax.second) {
                                minMax.second = animal->age;
                            }
                        }
                        cout << "Владелец: " << owner.first << endl;
                        for (const auto& typeMinMax : minMaxAge) {
                            cout << "   Вид: " << typeMinMax.first << ", Минимальный возраст: "
                                 << typeMinMax.second.first
                                 << ", Максимальный возраст: " << typeMinMax.second.second << endl;
                        }
                    }
                    break;
                }
                default:
                    cout << "Некорректный ввод. Пожалуйста, выберите существующий пункт меню." << endl;
            }
        }

        for (auto& owner : ownerAnimals) { // Освобождение памяти
            owner.second.clear();
        }
    }

    return 0;
}