/*
 * Make an expandable navigation-tree label open its node before following
 * the label link. The arrow retains Doxygen's expand/collapse behavior, while
 * the label expands a closed branch and retains normal link navigation.
 *
 * The listener runs in the capture phase so expansion occurs before
 * Doxygen's label handler navigates. Leaf labels and modified clicks retain
 * their normal link behavior.
 *
 * Unlike the default Doxygen initialization, the documentation landing page
 * explicitly opens the project root and its first child (normally
 * "Overview"), making the first section list visible immediately. On every
 * page, an expandable selected item is also opened after Doxygen synchronizes
 * the navigation tree with the current URL.
 */
(function () {
  "use strict";

  function isIndexPage() {
    const page = window.location.pathname.split("/").pop();
    return page === "" || page === "index.html";
  }

  function expandItem(item) {
    const expandToggle = item ? item.querySelector(":scope > a") : null;
    if (!expandToggle || !expandToggle.querySelector(".arrow")) {
      return false;
    }

    // Do not toggle an item closed if Doxygen has already expanded it while
    // synchronizing the tree with the current page.
    if (!expandToggle.querySelector(".arrowhead.opened")) {
      expandToggle.click();
    }
    return true;
  }

  function expandFirstBranch() {
    const rootItem = document.querySelector(
      "#nav-tree-contents > ul > li:first-child > .item"
    );
    if (!expandItem(rootItem)) {
      return false;
    }

    // Expand the first child below the project root (normally "Overview") so
    // its section list is visible when the landing page opens.
    const firstChildItem = document.querySelector(
      "#nav-tree-contents > ul > li:first-child > ul.children_ul " +
      "> li:first-child > .item"
    );
    if (!firstChildItem) {
      return false;
    }

    // A first child without descendants needs no further expansion.
    expandItem(firstChildItem);
    return true;
  }

  function expandSelectedItem() {
    const selectedItem = document.querySelector("#nav-tree .item.selected");
    if (!selectedItem) {
      return false;
    }

    // Leaf items require no action. For expandable items, expandItem() opens
    // the branch only when it is currently closed.
    expandItem(selectedItem);
    return true;
  }

  document.addEventListener("DOMContentLoaded", function () {
    const navTreeContents = document.getElementById("nav-tree-contents");
    if (!navTreeContents) {
      return;
    }

    if (isIndexPage() && !expandFirstBranch()) {
      // Doxygen constructs the navigation tree after DOMContentLoaded and may
      // load parts of it asynchronously. Observe the tree until its first
      // branch exists, expand it once, and then stop observing.
      const firstBranchObserver = new MutationObserver(function () {
        if (expandFirstBranch()) {
          firstBranchObserver.disconnect();
        }
      });
      firstBranchObserver.observe(navTreeContents, { childList: true, subtree: true });
    }

    if (!expandSelectedItem()) {
      // Selection is applied after nodes are created. Watch child additions
      // and class changes until the selected item is available.
      const selectedItemObserver = new MutationObserver(function () {
        if (expandSelectedItem()) {
          selectedItemObserver.disconnect();
        }
      });
      selectedItemObserver.observe(navTreeContents, {
        attributes: true,
        attributeFilter: ["class"],
        childList: true,
        subtree: true
      });
    }
  });

  document.addEventListener("click", function (event) {
    // Only augment an ordinary primary-button click. This preserves actions
    // such as Ctrl+click and Shift+click for opening or navigating links.
    if (event.button !== 0 || event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) {
      return;
    }

    const target = event.target;
    if (!(target instanceof Element)) {
      return;
    }

    const label = target.closest("#nav-tree .label");
    if (!label) {
      return;
    }

    // A leaf item has no expansion control and therefore continues to
    // navigate normally.
    const item = label.closest(".item");
    if (!item || !item.querySelector(":scope > a > .arrow")) {
      return;
    }

    // Open the branch only when it is closed. Do not cancel the label event:
    // Doxygen must still process it and follow the target link.
    expandItem(item);
  }, true);
}());
