package com.example.tutorial;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;

import com.example.tutorial.AddressBookProtos.AddressBook;
import com.example.tutorial.AddressBookProtos.Person;


public class PersonAdder {
  static Person promptForAddress(BufferedReader in, PrintStream out) throws NumberFormatException, IOException {
    Person.Builder builder = Person.newBuilder();
    
    out.print("Enter person ID:");
    builder.setId(Integer.parseInt(in.readLine()));
    
    out.print("Enter Name:");
    builder.setName(in.readLine());
    
    out.print("Enter Email (blank for none):");
    String email = in.readLine();
    if (!email.isEmpty()) {
      builder.setEmail(email);
    }
    
    while (true) {
      out.print("Enter a phone number (or leave blank to finish):");
      String number = in.readLine();
      if (number.isEmpty()) {
        break;
      }
      
      Person.PhoneNumber.Builder phoneBuilder = Person.PhoneNumber.newBuilder();
      phoneBuilder.setNumber(number);
      
      out.print("Mobile, Home, or Work?");
      String type = in.readLine();
      if (type.equalsIgnoreCase("mobile")) {
        phoneBuilder.setType(Person.PhoneType.MOBILE);
      } else if (type.equalsIgnoreCase("home")) {
        phoneBuilder.setType(Person.PhoneType.HOME);
      } else {
        phoneBuilder.setType(Person.PhoneType.WORK);
      }
      
      builder.addPhone(phoneBuilder);      
    }
    
    return builder.build();
  }

  /**
   * @param args
   * @throws IOException 
   * @throws FileNotFoundException 
   */
  public static void main(String[] args) throws FileNotFoundException, IOException {
    AddressBook.Builder addressBuilder = AddressBook.newBuilder();
    
    FileInputStream fis = new FileInputStream(Constants.fileName);
    addressBuilder.mergeFrom(fis);
    fis.close();
    
    Person person = PersonAdder.promptForAddress(new BufferedReader(new InputStreamReader(System.in)), System.out);
    
    addressBuilder.addPerson(person);
    
    AddressBook addressBook = addressBuilder.build();
    
    FileOutputStream fos = new FileOutputStream(Constants.fileName);
    addressBook.writeTo(fos);    
    
    fos.close();
    
    System.out.println("add a person successfully");
  }
}