$Id$

README file for the Faceted Search Drupal module.


Description
***********

The Faceted Search module provides a search API and a search interface for
allowing users to navigate content in such a way that they can rapidly get
acquainted with the scope and nature of the content, and never feel lost in the
data. More than a search interface, this is an information discovery tool.

The interface exposes metadata in such a way that users can build their queries
as they go, refining or expanding the current query, with results automatically
reflecting the current query. This interface also combines free-text search,
fully leveraging Drupal's search engine. It avoids complex search forms (which
break the navigation flow), and never offers facets that would lead to empty
result sets.

The most obvious metadata for faceted searches is provided by Drupal's taxonomy
module. However, Faceted Search's API allows developers to expose other
metadata, therefore providing more more facets to users for browsing content.


The package
***********

Faceted Search is in fact a bundle of modules.

- Faceted Search: Provides the search framework and API.

- Faceted Search UI: Provides the search user interface.

- Faceted Search Views: Allows to use Views to display the search results.

- Author Facet: Allows users to refine the current search based on content
  author.

- Content Type Facet: Allows users to refine the current search based on content
  type.

- Date Authored Facet: Allows users to refine the current search based on
  content creation date.

- Taxonomy Facets: Allows users to search content through taxonomy. Any
  vocabulary can become a facet that can be used to refine the current search.

Hopefully, many more facets will be developed. The API is meant to make it easy
to implement new facets.


Current status
**************

This module is undergoing development. The provided features and API might still
change without warning.

Although this code uses some object-oriented programming, it is still compatible
with PHP 4 at the moment.


Caution
*******

Faceted searches are database-intensive. If your server can barely keep up with
your traffic, this package will make things worst. Make sure to benchmark
performance before deploying this package on a busy site.


Requirements
************

- Drupal 5.x (http://drupal.org/project/drupal).

- MySQL 4.1 (or later version). 


Recommended modules
*******************

- Views (http://drupal.org/project/views): In combination with the Faceted
  Search Views module, the Views module can give you tremendous flexibility for
  displaying Faceted Search's results, and even for performing additional
  filtering of the search results.

- jQuery Update (http://drupal.org/project/jquery_update): If you wish to use
  Faceted Search UI's tooltips feature (for showing subcategories when hovering
  over a category in the guided search), it is strongly recommended to install
  the jQuery Update module. Make sure to read that module's installation
  instructions. If you don't use the tooltips feature, Faceted Search UI won't
  use jQuery at all, so in that case you would not need jQuery Update.

- Taxonomy hide (http://drupal.org/project/taxonomy_hide): If you use the
  Faceted Search UI's Related Categories block, you might want to remove
  Drupal's default terms listing when viewing a node. You could do that from
  your site's theme, but another way could be to use the Taxonomy hide module.


Known incompatibilities
***********************

- PostgreSQL: The Date Authored Facet module uses some MySQL-specific functions.
  Feel free to share patches to support PostgreSQL (or any other database). :-)


Installation
************

1. Extract the 'faceted_search' module directory, including all its
   subdirectories, into your Drupal modules directory.

2. Go to the Administer > Site building > Modules page, and enable the following
   modules:

   - Faceted Search
   - Faceted Search UI
   - At least one of the following modules (which are provided with Faceted Search):
     - Author Facet
     - Content Type Facet
     - Date Authored Facet
     - Taxonomy Facets
   - Search (Drupal core module)
   - Taxonomy (Drupal core module -- only needed if you intend to use Taxonomy Facets)

3. Go to the Administer > Site building > Blocks page, and enable the following
   blocks:

   - Faceted search / Current search
   - Faceted search / Keyword search
   - Faceted search / Guided search
   - Faceted search / Related categories
   - Faceted search / Sort options

   It is most intuitive for users to have the Current search block located above
   the Keyword search and Guided search blocks, and the Related categories and
   Sort options blocks displayed under the content.

4. Go to the Administer > Site configuration > Faceted Search page.

4a. If you have enabled the Author Facet module, click the Author Facet tab and
    check the roles you'd like to exclude from faceted searches.

4b. If you have enabled the Content Type Facet module, click the Content Type
    Facet tab and check the content types you'd like to use in faceted searches.

4c. If you have enabled the Date Authored Facet module, click the Date Authored
    Facet tab and select the date formats you'd like to use in faceted searches.

4d. If you have enabled the Taxonomy Facets module, click the Taxonomy Facets
    tab and check the vocabularies you'd like to use in faceted searches.

    If your site does not have vocabularies, you'll have to create them, and
    populate them with terms. See the Drupal handbook pages on taxonomy for more
    information (http://drupal.org/handbook/modules/taxonomy).

5. Click the Settings tab and assign weights and maximum number of categories to
   each facet. These options apply to the Current search and Guided search
   blocks.

6. Go to the Administer > User management > Access control page, and grant the
   "use faceted search" permission to the roles you intend to give access to
   faceted search.


Views integration
*****************

You may use Views to display search results through the Faceted Search Views
module. This allows to display results in an embedded view.

Results shown through a view might differ from those obtained from other display
styles, because the view might provide additional filters. A view must use the
"Faceted Search: In current search results" filter to become available to
Faceted Search as a display style.

Note also that, because it is embedded in Faceted Search's results page, the
view cannot use exposed filters or URL-based arguments.

Assuming that Faceted Search is already installed and configured on your site,
here are the step-by-step instructions to use a View to display search results:

1. Go to Administer > Site building > Views, and click Add to create a new view.

2. Enter a name for the view.

3. Choose to provide a Page view. Note that the "Title", "Use pager", "Empty
   text" and "Nodes per Page" options are ignored when the view is embedded
   within Faceted Search's results page.

4. In the Filters section, add the "Faceted Search: In current search results"
   filter. Then select Equals as operator, and Yes as value.

5. Optional: In the Sort Criteria section, add the "Faceted Search: Score"
   field. Then select the Descending order.

6. Save your new view.

7. Go to Administer > Site configuration > Faceted search, and select your new
   view in the "Display style for search results" field.

Note: If you provide a Page view, the Faceted Search filter and sort field will
be ignored when using the view directly from its URL.


Support
*******

For support requests, bug reports, and feature requests, please use Faceted
Search's issue queue on http://drupal.org/project/issues/faceted_search.

You may also contact the author for paid customizations to this module
(http://davidlesieur.com/contact).


Credits
*******

* Project initiated and developed by David Lesieur (http://davidlesieur.com,
  http://drupal.org/user/17157).

* Sponsored in part by Laboratoire NT2 (http://www.labo-nt2.uqam.ca) and Eyos BV
  (http://www.eyos.nl).

* The superb Flamenco search interface (http://flamenco.berkeley.edu) has
  provided, and still provides, most of the inspiration for this project.


See also
********

* Biblio Facets (http://drupal.org/project/biblio_facets): Integrates Faceted
  Search with Biblio (http://drupal.org/project/biblio) to navigate Biblio's
  types and fields as facets.

