package com.letsmidi.monsys;

public class FgwInfo {
  public final String name;
  public final String id;

  public FgwInfo(String name, String id) {
    this.name = name;
    this.id = id;
  }

  @Override
  public String toString() {
    return "FgwInfo [name=" + name + ", id=" + id + "]";
  }

}