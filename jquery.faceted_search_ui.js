// $Id$

/**
 * Provides tooltips with subcategories when hovering categories, through AHAH.
 */ 
jQuery.facetedSearchUI = {
  isActive : false, // True when hovering a category.
  cache : {}, // Cached HTML chunks.

  activate : function() {
    if (jQuery.facetedSearchUI.isActive || !this.href) {
      return;
    }
    
    // Extract the facet key and id from the class of the category's parent.
    var facetKey = null;
    var facetId = null;
    
    jQuery(this).parents('.faceted-search-facet').each(function() {
      var matches = jQuery(this).attr('class').match(/faceted-search-facet--([^ ]+)--([^ ]+)/);
      if (matches.length > 0) {
        facetKey = matches[1];
        facetId = matches[2];
      }
    });
    if (!facetKey || !facetId) {
      return;
    }

    // Derive the url from the category's url, which also contains the search text.
    var url = this.href.replace(/\/results\/na\//, '/categories/' + facetKey + '%3A' + facetId + '/');
    if (url != this.href) {
      jQuery.facetedSearchUI.isActive = true;
      
      // Prepare the tooptip.
      var elemPosition = jQuery(this).offset();
      var elemWidth = jQuery(this).outerWidth();
      var windowWidth = jQuery(window).width();
      var tooltip = jQuery('#faceted-search-tooltip');
      var tooltipWidth = tooltip.outerWidth();
      tooltip.css('top', elemPosition.top + 'px');
      if (elemPosition.left + elemWidth + tooltipWidth + 10 > windowWidth) {
        tooltip.css('left', (elemPosition.left - tooltipWidth - 10) + 'px');
        tooltip.addClass('faceted-search-tooltip-left'); // Might be useful for theming.
      }
      else {
        tooltip.css('left', (elemPosition.left + elemWidth + 10) + 'px');
        tooltip.addClass('faceted-search-tooltip-right'); // Might be useful for theming.
      }

      // Show the tooltip.
      var id = jQuery.facetedSearchUI.makeCacheId(url);
      if (id in jQuery.facetedSearchUI.cache) { // Show from the cache.
        jQuery.facetedSearchUI.show(jQuery.facetedSearchUI.cache[id]);
      }
      else { // Was not in the cache, load it from the server.
        jQuery.get(url, null, jQuery.facetedSearchUI.load);
      }
    }
  },
  
  deactivate : function() {
    if (jQuery.facetedSearchUI.isActive) {
      jQuery.facetedSearchUI.isActive = false;
      jQuery('#faceted-search-tooltip').hide().empty().removeClass('faceted-search-tooltip-left').removeClass('faceted-search-tooltip-right');
    }
  },

  load : function(data) {
    data = Drupal.parseJson(data);
    if (data.id) {
      // Cache the received HTML chunk and show it.
      jQuery.facetedSearchUI.cache[data.id] = data.content;
      jQuery.facetedSearchUI.show(data.content);
    }
  },
  
  show : function(chunk) {
    // Show the tooltip, but only if we were called back while the tooltip was
    // still active (i.e. only if the mouse did not move out).
    if (jQuery.facetedSearchUI.isActive && chunk.length > 0) {
      jQuery('#faceted-search-tooltip').empty().append(chunk).show();
    }
  },

  makeCacheId : function(url) {
    // Extract the search text to use as id.
    return url.substr(url.lastIndexOf('/') + 1);
  }
};

if (Drupal.jsEnabled) {
  jQuery(function() {
    // Insert the tooltip block.
    jQuery('body').append('<div id="faceted-search-tooltip"></div>');
    // Bind hover behavior on category links.
    jQuery('.faceted-search-category a').each(function() {
      // TODO: Some delay before sending request... This will save some requests. Consider jquery.hoverIntent.js
      jQuery(this).hover(jQuery.facetedSearchUI.activate, jQuery.facetedSearchUI.deactivate);
    });
    // Bind click behavior to force closing the tooptip, if ever needed.
    jQuery(this).click(jQuery.facetedSearchUI.deactivate);
  });
}

