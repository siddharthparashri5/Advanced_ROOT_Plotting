# Quick Reference Guide - Doxygen & ROOT Documentation

## Doxygen Comment Syntax Quick Reference

### Basic Comment Blocks

```cpp
/**
 * Javadoc style - use this for classes and detailed documentation
 */

/// Qt style - use this for brief one-liners

//! Alternative Qt style

/*!
 * Alternative Javadoc style
 */
```

### Class Documentation Template

```cpp
/**
 * @class ClassName
 * @brief One-line description of the class
 * 
 * @details
 * Detailed description spanning multiple lines.
 * Explain purpose, usage, and important details.
 * 
 * Key features:
 * - Feature 1
 * - Feature 2
 * - Feature 3
 * 
 * Example usage:
 * @code
 * ClassName obj(param);
 * obj.Method();
 * @endcode
 * 
 * @note Important notes about usage
 * @warning Warnings about limitations or pitfalls
 * @see RelatedClass, RelatedFunction()
 */
class ClassName : public BaseClass {
    // Implementation
    ClassDef(ClassName, 1)  // ROOT class definition
};
```

### Method Documentation Template

```cpp
/**
 * @brief Brief description of what method does
 * @param name1 Description of first parameter
 * @param name2 Description of second parameter  
 * @param name3 Description of third parameter (optional, default: value)
 * @return Description of return value
 * 
 * @details
 * Detailed explanation of algorithm, side effects,
 * preconditions, postconditions, etc.
 * 
 * @pre Preconditions that must be true before calling
 * @post Postconditions guaranteed after successful call
 * 
 * @throws ExceptionType When and why this exception is thrown
 * 
 * @note Additional notes
 * @warning Important warnings
 * 
 * @see OtherMethod(), RelatedClass
 * 
 * Example:
 * @code
 * ReturnType result = obj.Method(arg1, arg2);
 * if (result.IsValid()) {
 *     // Use result
 * }
 * @endcode
 */
ReturnType MethodName(Type1 name1, Type2 name2, Type3 name3 = default);
```

### Member Variable Documentation

```cpp
// Inline documentation (after declaration)
Int_t fCount;           ///< Number of items
Double_t fValue;        ///< Current value in range [0,1]
TString fName;          ///< Object name (max 256 chars)

// Block documentation (before declaration)
/**
 * @brief Complex member needing detailed explanation
 * 
 * @details
 * More information about this member,
 * valid ranges, units, etc.
 */
Double_t fComplexMember;
```

### Function Documentation Template

```cpp
/**
 * @brief Brief description
 * @param[in] input Input parameter (read-only)
 * @param[out] output Output parameter (modified)
 * @param[in,out] both Input and output parameter
 * @return Success status: 0=OK, <0=error code
 * 
 * @retval 0 Success
 * @retval -1 Invalid input
 * @retval -2 Computation failed
 */
Int_t FunctionName(const Type1& input, Type2& output, Type3& both);
```

## Common Doxygen Tags

### Structure and Organization

```cpp
/// @file filename.h - File documentation
/// @brief Brief file description
/// @author Your Name
/// @date 2026-02-04
/// @version 1.0

/// @namespace NamespaceName - Namespace documentation
/// @brief Brief namespace description

/// @defgroup groupname Group Name - Define documentation group
/// @brief Brief group description
/// @{
    // Members of the group
/// @}

/// @addtogroup groupname - Add to existing group
/// @{
    // Additional members
/// @}
```

### Parameter Documentation

```cpp
/// @param name Description
/// @param[in] name Input parameter
/// @param[out] name Output parameter  
/// @param[in,out] name Input/output parameter
/// @tparam T Template parameter
```

### Return Documentation

```cpp
/// @return Description of return value
/// @returns Alternative to @return
/// @retval value Description when this value returned
```

### Cross-References

```cpp
/// @see OtherFunction()
/// @see OtherClass::Method()
/// @see OtherClass
/// @ref PageName
/// @link ClassName::method() link text @endlink
```

### Special Markers

```cpp
/// @todo Add implementation for feature X
/// @bug Description of known bug
/// @deprecated Use NewFunction() instead
/// @since Version 2.0
/// @version 1.5.3
/// @date 2026-02-04
/// @author John Doe
/// @copyright MIT License
```

### Notes and Warnings

```cpp
/// @note Important note
/// @attention Pay attention to this
/// @warning This is dangerous
/// @remark Additional remark
```

### Code Examples

```cpp
/**
 * @code
 * // Full code example
 * MyClass obj;
 * obj.DoSomething();
 * @endcode
 * 
 * @code{.cpp}
 * // C++ highlighted example
 * std::vector<int> v = {1, 2, 3};
 * @endcode
 * 
 * @verbatim
 * Plain text output:
 * Line 1
 * Line 2
 * @endverbatim
 */
```

### Mathematical Formulas

```cpp
/**
 * Inline formula: \f$ E = mc^2 \f$
 * 
 * Display formula:
 * \f[
 *   f(x) = \frac{1}{\sigma\sqrt{2\pi}} 
 *          e^{-\frac{(x-\mu)^2}{2\sigma^2}}
 * \f]
 * 
 * Multiline:
 * \f{eqnarray*}{
 *   x &=& a + b \\
 *   y &=& c + d
 * \f}
 */
```

### Lists

```cpp
/**
 * Bullet list:
 * - Item 1
 * - Item 2
 *   - Nested item 2a
 *   - Nested item 2b
 * - Item 3
 * 
 * Numbered list:
 * 1. First step
 * 2. Second step
 * 3. Third step
 * 
 * Definition list:
 * Term 1
 *   : Definition of term 1
 * Term 2
 *   : Definition of term 2
 */
```

### Tables

```cpp
/**
 * Simple table:
 * | Header 1 | Header 2 | Header 3 |
 * |----------|----------|----------|
 * | Cell 1   | Cell 2   | Cell 3   |
 * | Cell 4   | Cell 5   | Cell 6   |
 * 
 * With alignment:
 * | Left | Center | Right |
 * |:-----|:------:|------:|
 * | L1   |   C1   |    R1 |
 * | L2   |   C2   |    R2 |
 */
```

### Images

```cpp
/**
 * Inline image: @image html diagram.png "Caption"
 * 
 * With width: @image html diagram.png "Caption" width=500px
 * 
 * Multiple formats:
 * @image html diagram.png "HTML version"
 * @image latex diagram.eps "LaTeX version" width=10cm
 */
```

## ROOT-Specific Documentation

### ClassDef and ClassImp

```cpp
// In header file (.h)
class MyClass : public TObject {
private:
    Int_t fData;

public:
    MyClass();
    virtual ~MyClass();
    
    ClassDef(MyClass, 1)  // ROOT class definition, version 1
};

// In implementation file (.cxx)
ClassImp(MyClass)  // ROOT class implementation

MyClass::MyClass() : TObject(), fData(0) {
    // Constructor
}
```

### ROOT Collection Classes

```cpp
/**
 * @brief Example class using ROOT collections
 */
class DataContainer : public TObject {
private:
    TObjArray *fObjects;     ///< Array of objects
    TList *fList;            ///< List of items
    THashTable *fTable;      ///< Hash table for fast lookup
    
public:
    /**
     * @brief Add object to container
     * @param obj Object to add (ownership transferred)
     */
    void Add(TObject* obj) { fObjects->Add(obj); }
    
    /**
     * @brief Get number of objects
     * @return Number of stored objects
     */
    Int_t GetSize() const { return fObjects->GetEntries(); }
    
    ClassDef(DataContainer, 1)
};
```

### ROOT Persistent Classes

```cpp
/**
 * @class PersistentData
 * @brief Data class that can be saved to ROOT files
 * 
 * @details
 * This class can be written to and read from .root files
 * using TFile::Write() and TFile::Get()
 */
class PersistentData : public TObject {
private:
    Double_t fValue;         ///< Data value
    TString fName;           ///< Data name
    std::vector<Double_t> fArray;  ///< Data array (needs special handling)
    
public:
    /// Default constructor (required for I/O)
    PersistentData() : TObject(), fValue(0.0) {}
    
    /**
     * @brief Constructor with parameters
     * @param name Object name
     * @param value Initial value
     */
    PersistentData(const char* name, Double_t value) 
        : TObject(), fValue(value), fName(name) {}
    
    // Getters and setters
    Double_t GetValue() const { return fValue; }
    void SetValue(Double_t v) { fValue = v; }
    
    /**
     * @brief Write object to file
     * @param filename Output ROOT file path
     * @param option Write option ("RECREATE", "UPDATE", etc.)
     * @return true if successful
     */
    bool WriteToFile(const char* filename, const char* option = "RECREATE");
    
    /**
     * @brief Read object from file
     * @param filename Input ROOT file path
     * @param objname Object name in file
     * @return Pointer to object, nullptr if failed
     */
    static PersistentData* ReadFromFile(const char* filename, const char* objname);
    
    ClassDef(PersistentData, 1)  // Version 1
};
```

### ROOT GUI Classes

```cpp
/**
 * @class MyGUI
 * @brief Example ROOT GUI window
 * 
 * @details
 * Demonstrates proper documentation of ROOT GUI components
 * including signal/slot connections
 */
class MyGUI : public TGMainFrame {
private:
    TGTextButton *fButton;       ///< Action button
    TGTextEntry *fEntry;         ///< Text input field
    TGLabel *fLabel;             ///< Display label
    
    /**
     * @brief Handle button click
     * 
     * @details
     * This slot is connected to the button's Clicked() signal
     */
    void HandleButton();
    
public:
    /**
     * @brief Constructor
     * @param p Parent window
     * @param w Window width
     * @param h Window height
     */
    MyGUI(const TGWindow *p, UInt_t w, UInt_t h);
    
    /// Destructor
    virtual ~MyGUI();
    
    /**
     * @brief Close window handler
     * 
     * @details
     * Called when user closes window.
     * Cleanup and exit application.
     */
    void CloseWindow();
    
    ClassDef(MyGUI, 0)  // GUI class (not persistent)
};
```

## Documentation Workflow Checklist

### Before Writing Code

- [ ] Understand the purpose and requirements
- [ ] Plan the class/function interface
- [ ] Consider what documentation users will need

### While Writing Code

- [ ] Add `@brief` for every public class/method
- [ ] Document all parameters with `@param`
- [ ] Document return values with `@return`
- [ ] Add usage examples for complex APIs

### After Writing Code

- [ ] Review documentation for clarity
- [ ] Add detailed `@details` sections
- [ ] Include `@note` and `@warning` where needed
- [ ] Add cross-references with `@see`
- [ ] Generate docs and check appearance

### Before Committing

- [ ] Run Doxygen without warnings
- [ ] Check generated HTML
- [ ] Verify examples compile
- [ ] Update user/developer guides if needed

## Common Patterns

### Factory Pattern Documentation

```cpp
/**
 * @class ObjectFactory
 * @brief Factory for creating objects of various types
 * 
 * @details
 * Implements the Factory design pattern for object creation.
 * Handles memory management and ensures proper initialization.
 * 
 * Example:
 * @code
 * ObjectFactory factory;
 * BaseObject* obj = factory.Create(kTypeA);
 * obj->DoSomething();
 * delete obj;  // Or use smart pointers
 * @endcode
 */
class ObjectFactory {
public:
    /// Object types that can be created
    enum ObjectType {
        kTypeA = 0,  ///< Type A object
        kTypeB = 1,  ///< Type B object
        kTypeC = 2   ///< Type C object
    };
    
    /**
     * @brief Create object of specified type
     * @param type Object type to create
     * @return Pointer to new object (caller owns)
     * @warning Caller must delete returned object
     */
    static BaseObject* Create(ObjectType type);
};
```

### Singleton Pattern Documentation

```cpp
/**
 * @class Singleton
 * @brief Thread-safe singleton class
 * 
 * @details
 * Implements thread-safe singleton pattern.
 * Only one instance exists throughout program lifetime.
 * 
 * Usage:
 * @code
 * Singleton& instance = Singleton::Instance();
 * instance.DoWork();
 * @endcode
 * 
 * @note Do not attempt to delete the instance
 * @warning Not copyable or assignable
 */
class Singleton {
private:
    /// Private constructor
    Singleton() {}
    
    /// Deleted copy constructor
    Singleton(const Singleton&) = delete;
    
    /// Deleted assignment operator
    Singleton& operator=(const Singleton&) = delete;
    
public:
    /**
     * @brief Get singleton instance
     * @return Reference to unique instance
     */
    static Singleton& Instance();
};
```

### Observer Pattern Documentation

```cpp
/**
 * @class Subject
 * @brief Subject in Observer pattern
 * 
 * @details
 * Maintains list of observers and notifies them of state changes.
 * Implements the Observer design pattern.
 * 
 * @see Observer
 */
class Subject {
private:
    std::vector<Observer*> fObservers;  ///< Registered observers
    
public:
    /**
     * @brief Register an observer
     * @param obs Observer to register (not owned)
     * @note Observer must remain valid until Detach() called
     */
    void Attach(Observer* obs);
    
    /**
     * @brief Unregister an observer
     * @param obs Observer to unregister
     */
    void Detach(Observer* obs);
    
    /**
     * @brief Notify all observers of state change
     * @details Calls Update() on each registered observer
     */
    void Notify();
};

/**
 * @class Observer
 * @brief Observer in Observer pattern
 * 
 * @details
 * Abstract base class for objects that observe subjects.
 * 
 * @see Subject
 */
class Observer {
public:
    /**
     * @brief Called when observed subject changes
     * @param subject Subject that changed
     */
    virtual void Update(Subject* subject) = 0;
};
```

## Quick Tips

1. **Start Simple**: Begin with `@brief`, then add details later
2. **Use Examples**: Code examples are worth a thousand words
3. **Be Consistent**: Follow one style throughout your project
4. **Update Regularly**: Keep docs synchronized with code
5. **Review Generated Output**: Always check the HTML/PDF output
6. **Link Related Items**: Use `@see` to connect related documentation
7. **Explain "Why"**: Not just "what" - explain design decisions
8. **Test Examples**: Make sure code examples actually compile
9. **Use Groups**: Organize related items with `@defgroup`
10. **Version Control**: Document version changes and API evolution

## Resources

- **Doxygen Manual**: https://www.doxygen.nl/manual/
- **ROOT Documentation**: https://root.cern.ch/doc/master/
- **Markdown Guide**: https://www.markdownguide.org/
- **LaTeX Math**: https://en.wikibooks.org/wiki/LaTeX/Mathematics

---

**Last Updated**: February 4, 2026
