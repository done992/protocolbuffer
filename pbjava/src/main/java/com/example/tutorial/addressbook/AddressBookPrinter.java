package com.example.tutorial.addressbook;

import java.io.FileInputStream;
import java.io.IOException;
import java.util.List;

import com.example.tutorial.addressbook.AddressBookProtos.AddressBook;
import com.example.tutorial.addressbook.AddressBookProtos.Person;
import com.example.tutorial.addressbook.AddressBookProtos.Person.PhoneNumber;


public class AddressBookPrinter {

  public void print(AddressBook book) {
    List<Person> persons = book.getPersonList();
    for (Person person : persons) {
      System.out.println("ID: " + person.getId());
      System.out.println("Name: " + person.getName());
      
      if (person.hasEmail()) {
        System.out.println("Email: " + person.getEmail());
      }
      
      List<PhoneNumber> phones = person.getPhoneList();
      for (PhoneNumber phone : phones) {
        Person.PhoneType type = phone.getType();
        if (type == Person.PhoneType.HOME) {
          System.out.print("Home: ");
        } else if (type == Person.PhoneType.MOBILE) {
          System.out.print("Mobile: ");
        } else {
          System.out.print("Work: ");
        }
        
        System.out.println(phone.getNumber());
      }
    }
  }
  
  
  public static void main(String[] args) throws IOException {
    FileInputStream fis = new FileInputStream(Constants.fileName);
    AddressBook addressBook = AddressBook.parseFrom(fis);
    
    AddressBookPrinter printer = new AddressBookPrinter();
    printer.print(addressBook);
    
    fis.close();
  }
}
