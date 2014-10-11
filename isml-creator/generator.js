
// Interactive Sonification Markup Language (Ishmael) Generator
// Author: James Walker jwwalker (a+) mtu (d0+) edu

// token lists //////////
var validTokens_variables = [
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
];

var validTokens_objects = [
  "left_hand", "right_hand", "either_hand", "both_hand",
  "left_foot", "right_foot", "either_foot", "both_feet", "head"
];

var validTokens_comparables = [
  "bpm",
  "cur_velocity_x", "cur_velocity_y", "cur_velocity_z", "cur_velocity_composite",
  "avg_velocity_x", "avg_velocity_y", "avg_velocity_z", "avg_velocity_composite",
  "acceleration_x", "acceleration_y", "acceleration_z", "acceleration_composite",
  "avg_proximity",
  "x_position", "y_position", "z_position", "elapsed_time_once", "elapsed_time_repeatable"
];

var validTokens_modifiables = [
  "bpm"
];

var validTokens_comparators = [
  "equal_to", "not_equal_to", "greater_than", "less_than"
];

var validTokens_keySignatures = [
  "c_major", "g_major", "d_major", "a_major", "e_major", "b_major", "fsharp_major", "csharp_major",
  "a_minor", "e_minor", "b_minor", "fsharp_minor", "csharp_minor", "gsharp_minor", "dsharp_minor",
  "asharp_minor"
];

var validTokens_timeSignatures = [
  "1/2", "2/2", "3/2", "4/2", "2/4", "3/4", "4/4", "2/8", "3/8", "4/8", "6/8", "8/8"
];

var validTokens_instruments = [
  "electronica", "rock", "orchestral"
];

var validTokens_operators = [
  "+", "-", "*", "/", "%", "^", "abs"
];

function verifyNumberParser(value) {
  var anum = /(^\d+$)|(^\d+\.\d+$)/;
  return anum.test(value);
}

function verifyNotes(value) {
  return (value.indexOf("notes:") === 0);
}
// end token lists //////////

// HashMap class //////////
var HashMap = function() {
  this._size = 0;
  this._map = {};
};

HashMap.prototype = {

  put: function(key, value) {
    if (!this.containsKey(key)) {
      this._size++;
    }
    this._map[key] = value;
  },
  
  remove: function(key) {
    if (this.containsKey(key)) {
      this._size--;
      var value = this._map[key];
      delete this._map[key];
      return value;
    } else {
      return null;
    }
  },
  
  containsKey: function(key) {
    return this._map.hasOwnProperty(key);
  },
  
  containsValue: function(value) {
    for (var key in this._map) {
      if (this._map.hasOwnProperty(key)) {
        if (this._map[key] === value) {
          return true;
        }
      }
    }

    return false;
  },
  
  get: function(key) {
    return this.containsKey(key) ? this._map[key] : null;
  },
  
  clear: function() {
    this._size = 0;
    this._map = {};
  },
  
  keys: function() {
    var keys = [];
    for (var key in this._map) {
      if (this._map.hasOwnProperty(key)) {
        keys.push(key);
      }
    }
    return keys;
  },
  
  values: function() {
    var values = [];
    for (var key in this._map) {
      if (this._map.hasOwnProperty(key)) {
        values.push(this._map[key]);
      }
    }
    return values;
  },
  
  size: function() {
    return this._size;
  }
};
// End HashMap class //////////

function swapArrayElements(array, idx1, idx2, displayId) {
  if (idx1 >= 0 && idx2 >= 0 && idx1 < array.length && idx2 < array.length) {
    tmp = array[idx1];
    array[idx1] = array[idx2];
    array[idx2] = tmp;
    generateHTML(displayId);
  }
}

var globalIdx = 0;

// data structures //////////
function ISML_Item() {
  this.activities = new HashMap();
  this.activityOrdering = [];
}

function ISML_Activity() {
  this.conditionSets = new HashMap();
  this.cSetOrdering = [];
  this.actions = new HashMap();
  this.actionOrdering = [];
}

function ISML_ConditionSet() {
  this.conditions = new HashMap();
  this.conditionOrdering = [];
}

function ISML_Condition() {
  this.type = "undefined";
  this.parameter1 = "none";
  this.parameter2 = "none";
  this.value1 = "none";
  this.value2 = "none";
  this.operator = "none";
}

function ISML_Action() {
  this.type = "undefined";
  this.parameter1 = "none";
  this.parameter2 = "none";
  this.parameter3 = "none";
  this.value1 = "none";
  this.value2 = "none";
  this.value3 = "none";
  this.operator = "none";
}

var items = new HashMap();
var itemOrdering = [];
// end data structures //////////

// script mutators //////////
function addItem(displayId) {
  items.put("item" + globalIdx, new ISML_Item());
  itemOrdering.push("item" + globalIdx);
  globalIdx++;
  generateHTML(displayId);
}

function deleteItem(itemKey, displayId) {
  var fieldToCheck = "confirmDelete" + itemKey;
  if (document.getElementById(fieldToCheck).checked) {
    items.remove(itemKey);
    itemOrdering.splice(itemOrdering.indexOf(itemKey), 1);
  } else {
    alert("As a safety measure, to confirm deletion, you must check the appropriate box.");
  }
  generateHTML(displayId);
}

function addActivity(itemKey, displayId) {
  items.get(itemKey).activities.put("activity" + globalIdx, new ISML_Activity());
  items.get(itemKey).activityOrdering.push("activity" + globalIdx);
  globalIdx++;
  generateHTML(displayId);
}

function deleteActivity(itemKey, activityKey, displayId) {
  var fieldToCheck = "confirmDelete" + activityKey;
  if (document.getElementById(fieldToCheck).checked) {
    items.get(itemKey).activities.remove(activityKey);
    items.get(itemKey).activityOrdering.splice(
      items.get(itemKey).activityOrdering.indexOf(activityKey), 1);
  } else {
    alert("As a safety measure, to confirm deletion, you must check the appropriate box.");
  }
  generateHTML(displayId);
}

function addAction(itemKey, activityKey, displayId) {
  items.get(itemKey).activities.get(activityKey).actions.put("action" + globalIdx, new ISML_Action());
  items.get(itemKey).activities.get(activityKey).actionOrdering.push("action" + globalIdx);
  globalIdx++;
  generateHTML(displayId);
}

function deleteAction(itemKey, activityKey, actionKey, displayId) {
  var fieldToCheck = "confirmDelete" + actionKey;
  if (document.getElementById(fieldToCheck).checked) {
    items.get(itemKey).activities.get(activityKey).actions.remove(actionKey);
    items.get(itemKey).activities.get(activityKey).actionOrdering.splice(
      items.get(itemKey).activities.get(activityKey).actionOrdering.indexOf(actionKey), 1);
  } else {
    alert("As a safety measure, to confirm deletion, you must check the appropriate box.");
  }
  generateHTML(displayId);
}

function addConditionSet(itemKey, activityKey, displayId) {
  items.get(itemKey).activities.get(activityKey).conditionSets.put("conditionset" + globalIdx,
    new ISML_ConditionSet());
  items.get(itemKey).activities.get(activityKey).cSetOrdering.push("conditionset" + globalIdx);
  globalIdx++;
  generateHTML(displayId);
}

function deleteConditionSet(itemKey, activityKey, cSetKey, displayId) {
  var fieldToCheck = "confirmDelete" + cSetKey;
  if (document.getElementById(fieldToCheck).checked) {
    items.get(itemKey).activities.get(activityKey).conditionSets.remove(cSetKey);
    items.get(itemKey).activities.get(activityKey).cSetOrdering.splice(
      items.get(itemKey).activities.get(activityKey).cSetOrdering.indexOf(cSetKey), 1);
  } else {
    alert("As a safety measure, to confirm deletion, you must check the appropriate box.");
  }
  generateHTML(displayId);
}

function addCondition(itemKey, activityKey, cSetKey, displayId) {
  items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditions.put(
    "condition" + globalIdx, new ISML_Condition());
  items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditionOrdering.push(
    "condition" + globalIdx);
  globalIdx++;
  generateHTML(displayId);
}

function deleteCondition(itemKey, activityKey, cSetKey, conditionKey, displayId) {
  var fieldToCheck = "confirmDelete" + conditionKey;
  if (document.getElementById(fieldToCheck).checked) {
    items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditions.remove(conditionKey);
    items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditionOrdering.splice(
      items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditionOrdering.indexOf(
        conditionKey), 1);
  } else {
    alert("As a safety measure, to confirm deletion, you must check the appropriate box.");
  }
  generateHTML(displayId);
}

function setConditionProperty(property, valueId, itemKey, activityKey, cSetKey, conditionKey, displayId) {
  var newValue = document.getElementById(valueId).value;
  items.get(itemKey).activities.get(activityKey).conditionSets.get(
    cSetKey).conditions.get(conditionKey)[property] = newValue;
  generateHTML(displayId);
}

function setActionProperty(property, valueId, itemKey, activityKey, actionKey, displayId) {
  var newValue = document.getElementById(valueId).value;
  items.get(itemKey).activities.get(activityKey).actions.get(actionKey)[property] = newValue;
  generateHTML(displayId);
}

function applyNotes(property, valueId, itemKey, activityKey, actionKey, displayId) {
  var newValue = "notes:" + document.getElementById(valueId).value.replace(/\s+/g, '');
  items.get(itemKey).activities.get(activityKey).actions.get(actionKey)[property] = newValue;
  generateHTML(displayId);
}

function verifyNumberAction(property, valueId, itemKey, activityKey, actionKey, displayId) {
  var value = document.getElementById(valueId).value;
  var anum = /(^\d+$)|(^\d+\.\d+$)/;
  if (anum.test(value)) {
    setActionProperty(property, valueId, itemKey, activityKey, actionKey, displayId);
  } else {
    alert("Please input a valid number.");
  }
}

function verifyNumberCondition(property, valueId, itemKey, activityKey, cSetKey, conditionKey, displayId) {
  var value = document.getElementById(valueId).value;
  var anum = /(^\d+$)|(^\d+\.\d+$)/;
  if (anum.test(value)) {
    setConditionProperty(property, valueId, itemKey, activityKey, cSetKey, conditionKey, displayId);
  } else {
    alert("Please input a valid number.");
  }
}

function constructActionParamMutator(parameterNum, itemKey, activityKey, actionKey, onlyModify) {
  var retStr = "";
  retStr += "<form>";
  retStr += "<select id=\"parameter" + parameterNum + actionKey + "\", "
    + "onchange=\"setActionProperty('parameter" + parameterNum + "', '"
    + "parameter" + parameterNum + actionKey + "', '" + itemKey + "', '" + activityKey
    + "', '" + actionKey + "', 'displayarea')\" class=\"nicedropdown\">";
  retStr += "<option selected>Select Token</option>";
  if (!onlyModify) retStr += "<option value=\"number\">Number</option>";
  retStr += "<option value=\"variable\">Variable</option>";
  if (!onlyModify) {
    for (var t = 0; t < validTokens_comparables.length; t++) {
      retStr += "<option value=\"" + validTokens_comparables[t] + "\">" + validTokens_comparables[t] + "</option>";
    }
  } else {
    for (var t = 0; t < validTokens_modifiables.length; t++) {
      retStr += "<option value=\"" + validTokens_modifiables[t] + "\">" + validTokens_modifiables[t] + "</option>";
    }
  }
  retStr += "</select>";
  retStr += "</form>";
  return retStr;
}

function constructActionValueMutator(valueNum, itemKey, activityKey, actionKey) {
  var retStr = "";
  retStr += "<textarea id=\"value" + valueNum + actionKey + "\" cols=\"10\" rows =\"1\" "
    + "style=\"width:75px; height:2em;\">1.0</textarea>";
  retStr += "<input value=\"Set value\" onclick=\"verifyNumberAction('value" + valueNum + "', '"
    + "value" + valueNum + actionKey + "', '" + itemKey + "', '" + activityKey + "', '"
    + actionKey + "', 'displayarea')\" type=\"button\" class=\"nicebutton\">";
  return retStr;
}

function constructActionVariableSetter(valueNum, itemKey, activityKey, actionKey) {
  var retStr = "";
  retStr += "<form>";
  retStr += "<select class=\"nicedropdown\" id=\"value" + valueNum + actionKey + "\", "
    + "onchange=\"setActionProperty('value" + valueNum + "', '" + "value" + valueNum + actionKey
    + "', '" + itemKey + "', '" + activityKey + "', '" + actionKey + "', 'displayarea')\">";
  retStr += "<option selected>Select Variable</option>";
  for (var n = 0; n < validTokens_variables.length; n++) {
    retStr += "<option value=\"" + validTokens_variables[n] + "\">"
      + validTokens_variables[n] + "</option>";
  }
  retStr += "</select>";
  retStr += "</form>";
  return retStr;
}

function constructConditionParamMutator(parameterNum, itemKey, activityKey, cSetKey, conditionKey) {
  var retStr = "";
  retStr += "<form>";
  retStr += "<select class=\"nicedropdown\" id=\"parameter" + parameterNum + conditionKey + "\", "
    + "onchange=\"setConditionProperty('parameter" + parameterNum + "', '"
    + "parameter" + parameterNum + conditionKey + "', '"
    + itemKey + "', '" + activityKey + "', '" + cSetKey + "', '"
    + conditionKey + "', 'displayarea')\">";
  retStr += "<option selected>Select Value to Check</option>";
  retStr += "<option value=\"number\">Number</option>";
  retStr += "<option value=\"variable\">Variable</option>";
  for (var t = 0; t < validTokens_comparables.length; t++) {
    retStr += "<option value=\"" + validTokens_comparables[t] + "\">" + validTokens_comparables[t] + "</option>";
  }
  retStr += "</select>";
  retStr += "</form>";
  return retStr;
}

function constructConditionValueMutator(valueNum, itemKey, activityKey, cSetKey, conditionKey) {
  var retStr = "";
  retStr += "<textarea id=\"value" + valueNum + conditionKey + "\" cols=\"10\" rows =\"1\" "
    + "style=\"width:75px; height:2em;\">1.0</textarea>";
  retStr += "<input value=\"Set value\" onclick=\"verifyNumberCondition('value" + valueNum + "', '"
    + "value" + valueNum + conditionKey + "', '" + itemKey + "', '" + activityKey + "', '"
    + cSetKey + "', '" + conditionKey + "', 'displayarea')\" type=\"button\" class=\"nicebutton\">";
  return retStr;
}

function constructConditionVariableSetter(valueNum, itemKey, activityKey, cSetKey, conditionKey) {
  var retStr = "";
  retStr += "<form>";
  retStr += "<select class=\"nicedropdown\" id=\"value" + valueNum + conditionKey + "\", "
    + "onchange=\"setConditionProperty('value" + valueNum + "', '"
    + "value" + valueNum + conditionKey + "', '"
    + itemKey + "', '" + activityKey + "', '" + cSetKey + "', '"
    + conditionKey + "', 'displayarea')\">";
  retStr += "<option selected>Select Variable</option>";
  for (var n = 0; n < validTokens_variables.length; n++) {
    retStr += "<option value=\"" + validTokens_variables[n] + "\">"
      + validTokens_variables[n] + "</option>";
  }
  retStr += "</select>";
  retStr += "</form>";
  return retStr;
}
// end script mutators //////////

function generateHTML(displayId) {
  var output = "";
  for (var i = 0; i < itemOrdering.length; i++) {
    output += "<div class=\"item\">"
    output += "<a href=\"javascript:swapArrayElements(itemOrdering, " + i + ", " + (i - 1)
      + ", 'displayarea')\">Move Up</a> ";
    output += "<a href=\"javascript:swapArrayElements(itemOrdering, " + i + ", " + (i + 1)
      + ", 'displayarea')\">Move Down</a> ";
    output += "<br />";

    output += "<input value=\"Delete Item\" onclick=\"deleteItem('"
      + itemOrdering[i] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\">";
    output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    output += "<input type=\"checkbox\" id=\"confirmDelete" + itemOrdering[i]
      + "\" value=\"Confirm Delete\">Check box to confirm deletion.<br />";

    output += "<input value=\"Add Activity\" onclick=\"addActivity('"
      + itemOrdering[i] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\"><br />";

    var activityKeys = items.get(itemOrdering[i]).activityOrdering;
    for (var a = 0; a < activityKeys.length; a++) {
      output += "<div class=\"activity\">"
      output += "<a href=\"javascript:swapArrayElements(items.get('" + itemOrdering[i]
        + "').activityOrdering, " + a + ", " + (a - 1) + ", 'displayarea')\">Move Up</a> ";
      output += "<a href=\"javascript:swapArrayElements(items.get('" + itemOrdering[i]
        + "').activityOrdering, " + a + ", " + (a + 1)
        + ", 'displayarea')\">Move Down</a> ";
      output += "<br />";

      output += "<input value=\"Delete Activity\" onclick=\"deleteActivity('"
        + itemOrdering[i] + "', '" + activityKeys[a] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\">";
      output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
      output += "<input type=\"checkbox\" id=\"confirmDelete" + activityKeys[a]
        + "\" value=\"Confirm Delete\">Check box to confirm deletion.<br />";

      output += "<input value=\"Add Condition Set\" onclick=\"addConditionSet('"
        + itemOrdering[i] + "', '" + activityKeys[a] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\"><br />";

      var cSetKeys = items.get(itemOrdering[i]).activities.get(activityKeys[a]).cSetOrdering;
      for (var s = 0; s < cSetKeys.length; s++) {
        output += "<div class=\"cset\">"

        output += "<input value=\"Delete Condition Set\" onclick=\"deleteConditionSet('"
          + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + cSetKeys[s]
          + "', 'displayarea');\" type=\"button\" class=\"nicebutton\">";
        output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        output += "<input type=\"checkbox\" id=\"confirmDelete" + cSetKeys[s]
          + "\" value=\"Confirm Delete\">Check box to confirm deletion.<br />";

        output += "<input value=\"Add Condition\" onclick=\"addCondition('" + itemOrdering[i] + "', '" +
          activityKeys[a] + "', '" + cSetKeys[s] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\"><br />";

        var conditionKeys = items.get(itemOrdering[i]).activities.get(activityKeys[a]).conditionSets.get(
          cSetKeys[s]).conditionOrdering;
        for (var d = 0; d < conditionKeys.length; d++) {
          output += "<div class=\"condition\">"
          /*output += "<a href=\"javascript:swapArrayElements(items.get('" + itemOrdering[i]
            + "').activities.get('" + activityKeys[a] + "').conditionSets.get('" + cSetKeys[s]
            + "').conditionOrdering, " + d + ", " + (d - 1)
            + ", 'displayarea')\">Move Up</a> ";
          output += "<a href=\"javascript:swapArrayElements(items.get('" + itemOrdering[i]
            + "').activities.get('" + activityKeys[a] + "').conditionSets.get('" + cSetKeys[s]
            + "').conditionOrdering, " + d + ", " + (d + 1)
            + ", 'displayarea')\">Move Down</a> ";
          output += "<br />"; /* */

          output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
          output += "<input value=\"Delete Condition\" onclick=\"deleteCondition('"
            + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + cSetKeys[s] + "', '" + conditionKeys[d]
            + "', 'displayarea');\" type=\"button\" class=\"nicebutton\">";
          output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
          output += "<input type=\"checkbox\" id=\"confirmDelete" + conditionKeys[d]
            + "\" value=\"Confirm Delete\">Check box to confirm deletion.<br />";

          // edit condition
          var currCondition = items.get(itemOrdering[i]).activities.get(activityKeys[a]).conditionSets.get(
            cSetKeys[s]).conditions.get(conditionKeys[d]);
          if (currCondition.type == "undefined") {
            output += "<form>";
            output += "<select class=\"nicedropdown\" id=\"conditionType" + conditionKeys[d] + "\", "
              + "onchange=\"setConditionProperty('type', '" + "conditionType" + conditionKeys[d] + "', '"
              + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + cSetKeys[s] + "', '"
              + conditionKeys[d] + "', 'displayarea')\">";
            output += "<option selected>Select Condition Type</option>";
            output += "<option value=\"object\">Object</option>";
            output += "<option value=\"comparison\">Comparison</option>";
            output += "</select>";
            output += "</form>";
          } else if (currCondition.type == "object") {
            if (currCondition.parameter1 == "none") {
              output += "Object is of type ";
              output += "<form>";
              output += "<select class=\"nicedropdown\" id=\"parameter1" + conditionKeys[d] + "\", "
                + "onchange=\"setConditionProperty('parameter1', '" + "parameter1" + conditionKeys[d]
                + "', '" + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + cSetKeys[s] + "', '"
                + conditionKeys[d] + "', 'displayarea')\">";
              output += "<option selected>Select Parameter Value</option>";
              for (var t = 0; t < validTokens_objects.length; t++) {
                output += "<option value=\"" + validTokens_objects[t] + "\">"
                  + validTokens_objects[t] + "</option>";
              }
              output += "</select>";
              output += "</form>";
            } else {
              output += "Object is of type " + currCondition.parameter1 + "<br />";
            }
          } else if (currCondition.type == "comparison") {
            // operand 1
            if (currCondition.parameter1 == "none") {
              output += constructConditionParamMutator(
                1, itemOrdering[i], activityKeys[a], cSetKeys[s], conditionKeys[d]);
            } else if (currCondition.parameter1 == "number") {
              if (currCondition.value1 == "none") {
                output += constructConditionValueMutator(
                  1, itemOrdering[i], activityKeys[a], cSetKeys[s], conditionKeys[d]);
              } else {
                output += currCondition.value1;
              }
            } else if (currCondition.parameter1 == "variable") {
              if (currCondition.value1 == "none") {
                output += constructConditionVariableSetter(
                  1, itemOrdering[i], activityKeys[a], cSetKeys[s], conditionKeys[d]);
              } else {
                output += currCondition.value1;
              }
            } else {
              output += currCondition.parameter1;
            }
            output += " ";
            // operator
            if (currCondition.operator == "none") {
              output += "<form>";
              output += "<select class=\"nicedropdown\" id=\"operator" + conditionKeys[d] + "\", "
                + "onchange=\"setConditionProperty('operator" + "', '"
                + "operator" + conditionKeys[d] + "', '"
                + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + cSetKeys[s] + "', '"
                + conditionKeys[d] + "', 'displayarea')\">";
              output += "<option selected>Select Comparison</option>";
              for (var t = 0; t < validTokens_comparators.length; t++) {
                output += "<option value=\"" + validTokens_comparators[t] + "\">"
                  + validTokens_comparators[t] + "</option>";
              }
              output += "</select>";
              output += "</form>";
            } else {
              output += currCondition.operator;
            }
            output += " ";
            // operand 2
            if (currCondition.parameter2 == "none") {
              output += constructConditionParamMutator(
                2, itemOrdering[i], activityKeys[a], cSetKeys[s], conditionKeys[d]);
            } else if (currCondition.parameter2 == "number") {
              if (currCondition.value2 == "none") {
                output += constructConditionValueMutator(
                  2, itemOrdering[i], activityKeys[a], cSetKeys[s], conditionKeys[d]);
              } else {
                output += currCondition.value2;
              }
            } else if (currCondition.parameter2 == "variable") {
              if (currCondition.value2 == "none") {
                output += constructConditionVariableSetter(
                  2, itemOrdering[i], activityKeys[a], cSetKeys[s], conditionKeys[d]);
              } else {
                output += currCondition.value2;
              }
            } else {
              output += currCondition.parameter2;
            }
          } // end if-comparison
          // end edit condition

          output += "</div>";
        } // end for loop through conditions

        output += "</div><br />";
      } // end for loop through csets

      output += "<input value=\"Add Action\" onclick=\"addAction('"+ itemOrdering[i]
        + "', '" + activityKeys[a] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\"><br />";

      var actionKeys = items.get(itemOrdering[i]).activities.get(activityKeys[a]).actionOrdering;
      for (var c = 0; c < actionKeys.length; c++) {

        output += "<div class=\"action\">"
        output += "<a href=\"javascript:swapArrayElements(items.get('" + itemOrdering[i]
          + "').activities.get('" + activityKeys[a] + "').actionOrdering, " + c + ", " + (c - 1)
          + ", 'displayarea')\">Move Up</a> ";
        output += "<a href=\"javascript:swapArrayElements(items.get('" + itemOrdering[i]
          + "').activities.get('" + activityKeys[a] + "').actionOrdering, " + c + ", " + (c + 1)
          + ", 'displayarea')\">Move Down</a> ";
        output += "<br />";

        output += "<input value=\"Delete Action\" onclick=\"deleteAction('"
          + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + actionKeys[c]
          + "', 'displayarea');\" type=\"button\" class=\"nicebutton\">";
        output += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        output += "<input type=\"checkbox\" id=\"confirmDelete" + actionKeys[c]
          + "\" value=\"Confirm Delete\">Check box to confirm deletion.<br />";

        // edit action
        var currAction = items.get(itemOrdering[i]).activities.get(activityKeys[a]).actions.get(actionKeys[c]);
        if (currAction.type == "undefined") {
          output += "<form>";
          output += "<select class=\"nicedropdown\" id=\"actionType" + actionKeys[c] + "\", "
            + "onchange=\"setActionProperty('type', '" + "actionType" + actionKeys[c] + "', '"
            + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + actionKeys[c]
            + "', 'displayarea')\">";
          output += "<option selected>Select Action Type</option>";
          output += "<option value=\"assignment\">Assignment</option>";
          output += "<option value=\"set_key\">Set Key Signature</option>";
          output += "<option value=\"set_time\">Set Time Signature</option>";
          output += "<option value=\"set_instruments\">Set Instruments</option>";
          output += "<option value=\"play_notes\">Play Notes</option>";
          output += "</select>";
          output += "</form>";
        } else if (currAction.type == "set_key") {
          if (currAction.value1 == "none") {
            output += "<form>";
            output += "<select class=\"nicedropdown\" id=\"value1" + actionKeys[c] + "\", "
              + "onchange=\"setActionProperty('value1', 'value1" + actionKeys[c] + "', '"
              + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + actionKeys[c] + "', 'displayarea')\">";
            output += "<option selected>Select Key Signature</option>";
            for (var t = 0; t < validTokens_keySignatures.length; t++) {
              output += "<option value=\"" + validTokens_keySignatures[t] + "\">"
                + validTokens_keySignatures[t] + "</option>";
            }
            output += "</select>";
            output += "</form>";
          } else {
            output += "Set key signature to " + currAction.value1;
          }
        } else if (currAction.type == "set_time") {
          if (currAction.value1 == "none") {
            output += "<form>";
            output += "<select class=\"nicedropdown\" id=\"value1" + actionKeys[c] + "\", "
              + "onchange=\"setActionProperty('value1', 'value1" + actionKeys[c] + "', '"
              + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + actionKeys[c] + "', 'displayarea')\">";
            output += "<option selected>Select Time Signature</option>";
            for (var t = 0; t < validTokens_timeSignatures.length; t++) {
              output += "<option value=\"" + validTokens_timeSignatures[t] + "\">"
                + validTokens_timeSignatures[t] + "</option>";
            }
            output += "</select>";
            output += "</form>";
          } else {
            output += "Set time signature to " + currAction.value1;
          }
        } else if (currAction.type == "set_instruments") {
          if (currAction.value1 == "none") {
            output += "<form>";
            output += "<select class=\"nicedropdown\" id=\"value1" + actionKeys[c] + "\", "
              + "onchange=\"setActionProperty('value1', 'value1" + actionKeys[c] + "', '"
              + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + actionKeys[c] + "', 'displayarea')\">";
            output += "<option selected>Select Instrument Set</option>";
            for (var t = 0; t < validTokens_instruments.length; t++) {
              output += "<option value=\"" + validTokens_instruments[t] + "\">"
                + validTokens_instruments[t] + "</option>";
            }
            output += "</select>";
            output += "</form>";
          } else {
            output += "Set instrument set to " + currAction.value1;
          }
        } else if (currAction.type == "play_notes") {
          if (currAction.value1 == "none") {
            output += "<textarea type=\"text\" id=\"" + actionKeys[c] + "input\" "
              + "rows=\"1\" cols=\"40\" style=\"width:300px; height:2em;\"></textarea>"
              + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input value=\"Enter value\""
              + "id=\"" + actionKeys[c] + "enterValue\"" + " onclick="
              + "\"applyNotes('value1', '" + actionKeys[c] + "input"
              + "', '" + itemOrdering[i] + "', '" + activityKeys[a] + "', '"
              + actionKeys[c] + "', 'displayarea');\" type=\"button\" class=\"nicebutton\">";
          } else {
            output += "Play " + currAction.value1;
          }
        } else if (currAction.type == "assignment") {
          // lhs
          if (currAction.parameter1 == "none") {
            output += constructActionParamMutator(1, itemOrdering[i], activityKeys[a], actionKeys[c], true);
          } else if (currAction.parameter1 == "number") {
            if (currAction.value1 == "none") {
              output += constructActionValueMutator(1, itemOrdering[i], activityKeys[a], actionKeys[c]);
            } else {
              output += currAction.value1;
            }
          } else if (currAction.parameter1 == "variable") {
            if (currAction.value1 == "none") {
              output += constructActionVariableSetter(1, itemOrdering[i], activityKeys[a], actionKeys[c]);
            } else {
              output += currAction.value1;
            }
          } else {
            output += currAction.parameter1;
          }
          output += " = ";
          // operand 1
          if (currAction.parameter2 == "none") {
            output += constructActionParamMutator(2, itemOrdering[i], activityKeys[a], actionKeys[c], false);
          } else if (currAction.parameter2 == "number") {
            if (currAction.value2 == "none") {
              output += constructActionValueMutator(2, itemOrdering[i], activityKeys[a], actionKeys[c]);
            } else {
              output += currAction.value2;
            }
          } else if (currAction.parameter2 == "variable") {
            if (currAction.value2 == "none") {
              output += constructActionVariableSetter(2, itemOrdering[i], activityKeys[a], actionKeys[c]);
            } else {
              output += currAction.value2;
            }
          } else {
            output += currAction.parameter2;
          }
          output += " ";
          // operator
          if (currAction.operator == "none") {
            output += "<form>";
            output += "<select class=\"nicedropdown\" id=\"operator" + actionKeys[c] + "\", "
              + "onchange=\"setActionProperty('operator" + "', '" + "operator" + actionKeys[c] + "', '"
              + itemOrdering[i] + "', '" + activityKeys[a] + "', '" + actionKeys[c] + "', 'displayarea')\">";
            output += "<option selected>Select Operation</option>";
            for (var t = 0; t < validTokens_operators.length; t++) {
              output += "<option value=\"" + validTokens_operators[t] + "\">" + validTokens_operators[t]
                + "</option>";
            }
            output += "</select>";
            output += "</form>";
          } else {
            output += currAction.operator;
          }
          output += " ";
          // operand 2
          if (currAction.parameter3 == "none") {
            output += constructActionParamMutator(3, itemOrdering[i], activityKeys[a], actionKeys[c], false);
          } else if (currAction.parameter3 == "number") {
            if (currAction.value3 == "none") {
              output += constructActionValueMutator(3, itemOrdering[i], activityKeys[a], actionKeys[c]);
            } else {
              output += currAction.value3;
            }
          } else if (currAction.parameter3 == "variable") {
            if (currAction.value3 == "none") {
              output += constructActionVariableSetter(3, itemOrdering[i], activityKeys[a], actionKeys[c]);
            } else {
              output += currAction.value3;
            }
          } else {
            output += currAction.parameter3;
          }
        } // end if-assignment
        // end edit action

        output += "</div>";
      } // end for loop through actions

      output += "</div><br />";
    } // end for loop through activities

    output += "</div><br />";
  } // end for loop through items

  output += "<br />";
  output += createDownload();
  document.getElementById(displayId).innerHTML = output;
}

function createDownload() {
  var retStr = "<a href=\"data:text/plain;charset=utf-8,";

  for (var i = 0; i < itemOrdering.length; i++) {
    retStr += "<item>%0D%0A";
    var activityKeys = items.get(itemOrdering[i]).activityOrdering;
    for (var a = 0; a < activityKeys.length; a++) {
      retStr += "<activity>%0D%0A";  
      var cSetKeys = items.get(itemOrdering[i]).activities.get(activityKeys[a]).cSetOrdering;
      if (cSetKeys.length > 0) retStr += "<if>%0D%0A";  
      for (var s = 0; s < cSetKeys.length; s++) {
        var conditionKeys = items.get(itemOrdering[i]).activities.get(activityKeys[a]).conditionSets.get(
          cSetKeys[s]).conditionOrdering;
        for (var d = 0; d < conditionKeys.length; d++) {
          var currCondition = items.get(itemOrdering[i]).activities.get(activityKeys[a]).conditionSets.get(
            cSetKeys[s]).conditions.get(conditionKeys[d]);
          if (currCondition.operator != "none") retStr += currCondition.operator + " ";
          if (currCondition.value1 != "none") retStr += currCondition.value1 + " ";
          else if (currCondition.parameter1 != "none") retStr += currCondition.parameter1 + " ";
          if (currCondition.value2 != "none") retStr += currCondition.value2;
          else if (currCondition.parameter2 != "none") retStr += currCondition.parameter2;
          retStr += "%0D%0A";
        } // end conditions loops
        if (s < cSetKeys.length - 1) retStr += "<or>%0D%0A";
      } // end condition sets loop
      if (cSetKeys.length > 0) retStr += "<then>%0D%0A";
      var actionKeys = items.get(itemOrdering[i]).activities.get(activityKeys[a]).actionOrdering;
      for (var c = 0; c < actionKeys.length; c++) {
        var currAction = items.get(itemOrdering[i]).activities.get(activityKeys[a]).actions.get(actionKeys[c]);
        if (currAction.type == "assignment") retStr += "= ";
        if (currAction.value1 != "none") retStr += currAction.value1 + " ";
        else if (currAction.parameter1 != "none") retStr += currAction.parameter1 + " ";
        if (currAction.operator != "none") retStr += currAction.operator + " ";
        else if (currAction.value2 != "none") retStr += "assign ";
        if (currAction.value2 != "none") retStr += currAction.value2 + " ";
        else if (currAction.parameter2 != "none") retStr += currAction.parameter2 + " ";
        if (currAction.value3 != "none") retStr += currAction.value3;
        else if (currAction.parameter3 != "none") retStr += currAction.parameter3;
        retStr += "%0D%0A";
      } // end actions loop
      if (cSetKeys.length > 0) retStr += "</if>%0D%0A";
      retStr += "</activity>%0D%0A";
    } // end activities loop
    retStr += "</item>%0D%0A";
  } // end items loop

  retStr += "\" target=\"_blank\">Download ISML File</a>";
  return retStr;
}

function loadScript(inputId, checkboxId, displayId) {
  globalIdx = 0;
  items = new HashMap();
  itemOrdering = [];
  if (document.getElementById(checkboxId).checked) {
    var input = document.getElementById(inputId).value.replace( new RegExp( "\\n", "g" ), "`" ).split("`");
    for (var i = 0; i < input.length; i++) input[i] = input[i].trim();
    var l = 0;
    while (l < input.length) {
      var itemKey = "";
      var activityKey = "";
      var cSetKey = "";
      var conditionKey = "";
      var actionKey = "";
      var line = "";
      if (input[l] == "<item>") {
        itemKey = "item" + globalIdx;
        items.put(itemKey, new ISML_Item());
        itemOrdering.push(itemKey);
        globalIdx++;
        l++;
        while (input[l] != "</item>") {
          if (input[l] == "<activity>") {
            activityKey = "activity" + globalIdx;
            items.get(itemKey).activities.put(activityKey, new ISML_Activity());
            items.get(itemKey).activityOrdering.push(activityKey);
            globalIdx++;
            l++;
            while (input[l] != "</activity>") {
              while (input[l] == "<if>" || input[l] == "<or>") {
                cSetKey = "conditionset" + globalIdx;
                items.get(itemKey).activities.get(activityKey).conditionSets.put(
                  cSetKey, new ISML_ConditionSet());
                items.get(itemKey).activities.get(activityKey).cSetOrdering.push(cSetKey);
                globalIdx++;
                l++;
                while (input[l] != "<then>" && input[l] != "<or>") {
                  conditionKey = "condition" + globalIdx;
                  items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditions.put(
                    conditionKey, new ISML_Condition());
                  items.get(itemKey).activities.get(activityKey).conditionSets.get(
                    cSetKey).conditionOrdering.push(conditionKey);
                  globalIdx++;
                  // parse condition
                  var currCondition = items.get(itemKey).activities.get(activityKey).conditionSets.get(
                    cSetKey).conditions.get(conditionKey);
                  line = input[l].split(/\s/);
                  if (validTokens_objects.indexOf(line[0]) != -1) {
                    currCondition.type = "object";
                    currCondition.parameter1 = line[0];
                  } else {
                    currCondition.type = "comparison";
                    // parameter 1
                    if (validTokens_variables.indexOf(line[1]) != -1) { // match variable 
                      currCondition.parameter1 = "variable";
                      currCondition.value1 = line[1];
                    } else if (validTokens_comparables.indexOf(line[1]) != -1) { // match comparable
                      currCondition.parameter1 = line[1];
                    } else if (verifyNumberParser(line[1])) { // match number
                      currCondition.parameter1 = "number";
                      currCondition.value1 = line[1];
                    }
                    // comparator
                    currCondition.operator = line[0];
                    // parameter 2
                    if (validTokens_variables.indexOf(line[2]) != -1) { // match variable
                      currCondition.parameter2 = "variable";
                      currCondition.value2 = line[2];
                    } else if (validTokens_comparables.indexOf(line[2]) != -1) { // match comparable
                      currCondition.parameter2 = line[2];
                    } else if (verifyNumberParser(line[2])) { // match number
                      currCondition.parameter2 = "number";
                      currCondition.value2 = line[2];
                    }
                  }
                  items.get(itemKey).activities.get(activityKey).conditionSets.get(cSetKey).conditions.put(
                    conditionKey, currCondition);
                  if (l < input.length - 1 && (input[l + 1] == "<then>" || input[l + 1] == "<or>")) break;
                  l++;
                } // end while input[l] != <then>/<or>
                l++;
              } // end while input[l] == <if>/<or>
              if (input[l] == "<then>") l++;
              while (isAction(input[l])) {
                actionKey = "action" + globalIdx;
                items.get(itemKey).activities.get(activityKey).actions.put(actionKey, new ISML_Action());
                items.get(itemKey).activities.get(activityKey).actionOrdering.push(actionKey);
                globalIdx++;
                var currAction = items.get(itemKey).activities.get(activityKey).actions.get(actionKey);
                line = input[l].split(/\s/);
                if (validTokens_keySignatures.indexOf(line[0]) != -1) {
                  currAction.type = "set_key";
                  currAction.value1 = line[0];
                } else if (validTokens_timeSignatures.indexOf(line[0]) != -1) {
                  currAction.type = "set_time";
                  currAction.value1 = line[0];
                } else if (validTokens_instruments.indexOf(line[0]) != -1) {
                  currAction.type = "set_instruments";
                  currAction.value1 = line[0];
                } else if (verifyNotes(line[0])) {
                  currAction.type = "play_notes";
                  currAction.value1 = line[0];
                } else {
                  currAction.type = "assignment";
                  // parameter 1 -- needed for all assignments
                  if (validTokens_variables.indexOf(line[1]) != -1) { // match variable
                    currAction.parameter1 = "variable";
                    currAction.value1 = line[1];
                  } else if (validTokens_comparables.indexOf(line[1]) != -1) { // match comparable
                    currAction.parameter1 = line[1];
                  } //else if (verifyNumberParser(line[1])) { // match number
                    //currAction.parameter1 = "number";
                    //currAction.value1 = line[1];
                  //}
                  // parameter 2 -- needed for [= a assign b] and [= a op b c]
                  if (line[2] == "assign" || line.length == 5) {
                    var loc = 3;
                    if (validTokens_variables.indexOf(line[loc]) != -1) { // match variable
                      currAction.parameter2 = "variable";
                      currAction.value2 = line[loc];
                    } else if (validTokens_comparables.indexOf(line[loc]) != -1) { // match comparable
                      currAction.parameter2 = line[loc];
                    } else if (verifyNumberParser(line[loc])) { // match number
                      currAction.parameter2 = "number";
                      currAction.value2 = line[loc];
                    }
                  }
                  // operator -- needed for [= a op b c] and [= a op c]
                  if (line[2] != "assign") currAction.operator = line[2];
                  // parameter 3 -- needed for [= a op b c] and [= a op c]
                  if (line[2] != "assign") {
                    var loc = 4;
                    if (line.length == 4) loc = 3;
                    if (validTokens_variables.indexOf(line[loc]) != -1) { // match variable
                      currAction.parameter3 = "variable";
                      currAction.value3 = line[loc];
                    } else if (validTokens_comparables.indexOf(line[loc]) != -1) { // match comparable
                      currAction.parameter3 = line[loc];
                    } else if (verifyNumberParser(line[loc])) { // match number
                      currAction.parameter3 = "number";
                      currAction.value3 = line[loc];
                    }
                  }
                }
                items.get(itemKey).activities.get(activityKey).actions.put(actionKey, currAction);
                if (l < input.length - 1 && !isAction(input[l+1])) break;
                l++;
              } // end while isAction(input[l])
              l++;
            } // end while input[l] != </activity>
          } // end if input[l] == <activity>
          l++;
        } // end while input[l] != </item>
      } // end if input[l] == <item>
      l++;
    } // end while
    generateHTML(displayId);
  } else {
    alert("As a safety measure, to confirm load, you must check the appropriate box.");
  }
}

function isAction(lineIn) {
  var line = lineIn.split(/\s/);
  if (validTokens_keySignatures.indexOf(line[0]) != -1) return true;
  if (validTokens_timeSignatures.indexOf(line[0]) != -1) return true;
  if (validTokens_instruments.indexOf(line[0]) != -1) return true;
  if (verifyNotes(line[0])) return true;
  if (line.length >= 4 && line[0] == "=") return true;
  return false;
}

