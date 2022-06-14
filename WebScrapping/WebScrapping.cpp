#include <iostream>
#include <fstream>
#include <cpr/cpr.h>
#include <gumbo.h>
#include <string>
#include <windows.h>
#include <shellapi.h>
#include <QString>
using namespace std;
using namespace cpr;


ofstream writeText("links.txt");
string extract_html_page()
{
    //Put any link
    Url url = Url{ "" };
    Response response = Get(url);
    return response.text;
}
void search_for_title(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    if (node->v.element.tag == GUMBO_TAG_H1)
    {
        GumboNode* title_text = static_cast<GumboNode*>(node->v.element.children.data[0]);
        cout << title_text->v.text.text << "\n";
        return;
    }
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++)
        search_for_title(static_cast<GumboNode*>(children->data[i]));
}
void search_for_spec_content(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    if (node->v.element.tag == GUMBO_TAG_DIV) {
        GumboAttribute* spec = gumbo_get_attribute(&node->v.element.attributes, "class");
        if (spec) {
            if (string(spec->value) == "summary")
            {
                QString strTitle;
                GumboNode* title_text = static_cast<GumboNode>*)(node->v.element.children.data[0]);

            }          
            /*GumboNode* div_text = static_cast<GumboNode*>(node->v.element.children.data[0]);
            cout << div_text->v.text.text << "\n";
            return;*/
        }
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++){
        search_for_spec_content(static_cast<GumboNode*>(children->data[i]));}

    /*if (node->v.element.tag == GUMBO_TAG_DIV)
    {
        GumboAttribute* spec = gumbo_get_attribute(&node->v.element.attributes, "class");
        if (spec->value == "summary")
            if (node->v.element.tag == GUMBO_TAG_P)
            {
                GumboNode* title_text = static_cast<GumboNode*>(node->v.element.children.data[0]);
                cout << title_text->v.text.text<< "\n";
                return;
            };

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; i++)
        {
            search_for_spec_content(static_cast<GumboNode*>(children->data[i]));
        }
    }*/

}

const char* find_attribute(GumboNode* current_node, GumboTag element_tag_type,
    char* element_term_key, char* element_term_value, char* desired_result_key)
{
    const char* lt_token = NULL;

    // Return NULL if it is in WHITESPACE
    if (current_node->type != GUMBO_NODE_ELEMENT)
    {
        return NULL;
    }

    // Set the element's term key, 
    // e.g. if we need to find something like <input name="foobar"> then element search term key is "name",
    //      and element search value is "foobar"
    GumboAttribute* lt_attr = gumbo_get_attribute(&current_node->v.element.attributes, element_term_key);

    if (lt_attr != NULL && current_node->v.element.tag == element_tag_type && (strcmp(lt_attr->value, element_term_value) == 0))
    {
        lt_token = gumbo_get_attribute(&current_node->v.element.attributes, desired_result_key)->value;
        return lt_token;
    }

    GumboVector* children = &current_node->v.element.children;

    for (unsigned int i = 0; i < children->length; ++i)
    {
        lt_token = find_attribute(static_cast<GumboNode*>(children->data[i]), element_tag_type,
            element_term_key, element_term_value, desired_result_key);

        // Force stop and return if it gets a non-null result.
        if (lt_token != NULL)
        {
            return lt_token;
        }
    }
}

void search_for_links(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (href)
            cout << href->value << "\n";
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++)
    {
        search_for_links(static_cast<GumboNode*>(children->data[i]));
    }
    
}
void search_for_links_to_txt(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;
    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (href)
        {
            string link = href->value;
            if (link.rfind("/wiki") == 0)
                writeText << "article," << link << "\n";
            else if (link.rfind("#cite") == 0)
                writeText << "cite," << link << "\n";
            else
                writeText << "other," << link << "\n";
        }
    }
}
int main()
{
    string page_content = extract_html_page();
    GumboOutput* parsed_response = gumbo_parse(page_content.c_str());
    search_for_spec_content(parsed_response->root);

    /*search_for_title(parsed_response->root);
    writeText << "type,link" << "\n";
    search_for_links(parsed_response->root);
    writeText.close();
    // free the allocated memory
    */
    gumbo_destroy_output(&kGumboDefaultOptions, parsed_response);
}

